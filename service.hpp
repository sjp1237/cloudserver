#pragma once
#include"FileInfoManger.hpp"
#include"httplib.h"

/*通信模块
 *1.上传文件的功能
 * 如果http请求是 Post /upload 则为上传文件
 *2.下载功能
  如果Getv /download/(.*),则为下载功能
 */
namespace sjp{
  class Service{
    private:
      std::string ip;
      int port;
      httplib::Server server;
      static std::mutex lock;
      static Service* instant;
    private:
      Service(){
        ip=sjp::Config::GetInstant()->GetIp();
        port=sjp::Config::GetInstant()->GetServerPort();
      }

      Service(Service&)=delete;
    private:
      static void UpLoad(const httplib::Request& req,httplib::Response& rep)
      {
        /* 1.判断是否有文件上传
         * 2.如果有，则获取该文件的名称和内容
         * 3.在backdir中创建一个文件
         * 4.将文件信息存储到FileInfoManger中
         * */
        //判断是否有文件上传
        std::cout<<"upload begin"<<endl;
          auto res=req.has_file("file");
          if(res==false){
            rep.status=400;
            return;
          } 
          
          const auto& file=req.get_file_value("file");
          std::string filepath=sjp::Config::GetInstant()->GetBackDir()+file.filename;
          std::string body=file.content;

          sjp::FileUtil fu(filepath);
          fu.SetContent(body);

          sjp::FileInfo fileinfo(filepath);
          sjp::FileInfoManger::GetInstant()->Insert(fileinfo);  
          sjp::FileInfoManger::GetInstant()->Storage();
          ListShow(req,rep);
      }

      static std::string mtimeToString(time_t mtime){
        std::string res= ctime(&mtime);
        return res;
      }
      static void ListShow(const httplib::Request& req,httplib::Response& rep){
        cout<<"listshow"<<endl;
        std::vector<sjp::FileInfo> arry;
        sjp::FileInfoManger::GetInstant()->GetAllInfo(arry);
        std::stringstream ss;

        //
          ss<<"<!DOCTYPE html>"<<"<html><meta charset=\"UTF-8\"><head><title>Download</title></head><body><h1>Download</h1><table>";  
                      ss<<"<tr><td><h3>文件名</h3></a></td>";    
            ss<<"<td align=\"left\">"<<"<h3>最近修改时间</h3>"<<"</td>";                                             
            ss<<"<td align=\"right\">"<<"<h3>文件大小</h3>"<<"</td></tr>"; 
        for(int i=0;i<arry.size();i++){
          //>
          /*<td><a href="/download/test.txt">test.txt</a></td>
            <td align="right">2021-12-29 10:10:10</td>
            <td align="right">28k</td>
           * */
          std::string path=arry[i].back_path;
          sjp::FileUtil fu(path);
          std::string atimestr=mtimeToString(arry[i].modify_time);
          ss<<"<tr><td><a href=\"/download/"<<fu.GetFilename()<<"\">"<<fu.GetFilename()<<"</a></td>";
          ss<<"<td align=\"right\">"<<atimestr<<"    </td>";
          ss<<"<td align=\"right\">"<<arry[i].file_size/1024<<"k"<<"</td></tr>";
        }
        //>
        ss<<"</table></body></html>";
        ss<<"<form action=\"http://119.23.41.13:8081/upload\" method=\"post\" enctype=\"multipart/form-data\"><div><input type=\"file\" name=\"file\"></div><div><input type=\"submit\" value=\"上传\"></div></form>";
        rep.body=ss.str();
        rep.status=200;
        rep.set_header("Content-Type","text/html;");
      }

      //文件名-文件大小-最后一次修改时间
      static  std::string GetEtag(std::string filename){
         sjp::FileUtil fu(filename);
         std::string etag=fu.GetFilename();
         etag+="-";
         etag+=std::to_string(fu.GetFileSize());
         etag+="-";
         etag+=std::to_string(fu.GetFileModfityTime());
         return etag;
      }

      static void DownLoad(const httplib::Request& req,httplib::Response& rep)
      {
        /*1.判断请求中的url是否在文件列表中
          2.如果存在,则获取文件的fileinfo信息,判断该文件是否为热点文件
             1.如果该文件是热点文件，直接从backdir中的文件发送給客户端
             2.如果该文件是非热点文件，需要先将文件解压到backdir中，删除packdir中的压缩文件
               并将文件发送給客户端
         3.如果不存在，则在backdir中查找是否有该资源，如果有则将该文件返回給客户端，并将文件信息存储到
           backinfofile中
         */
        std::string url=req.path;
        sjp::FileInfo fileinfo;
        sjp::FileInfoManger::GetInstant()->GetoneByURL(url,fileinfo);
        if(!sjp::FileInfoManger::GetInstant()->Exist(fileinfo.back_path)){
          /* sjp::FileUtil fu(back_dir); 
           std::vector<std::string> arry;
           fu.GetPathInDir(arry);
           */
          rep.status=404;
          return ;
        }

        if(fileinfo.pack_sign==true)
        {
          //解压缩
          sjp::FileUtil fu(fileinfo.pack_path);
          fu.UnpackFile(fileinfo.back_path);
          fileinfo.pack_sign=false;
          sjp::FileInfoManger::GetInstant()->Insert(fileinfo);
          fu.Remove();
        }


        bool flag=false;
        //判断是否为断点续传
        if(req.has_header("If-Range")){
          //断点续传
          std::string oldetag=req.get_header_value("If-Range"); 
          if(oldetag==GetEtag(fileinfo.back_path)){
             //需要断点续传
             cout<<"oldetag: "<<oldetag<<endl;
             cout<<"etag: "<<GetEtag(fileinfo.back_path)<<endl;
             flag=true;
          }
        }

        sjp::FileUtil fu(fileinfo.back_path);
        //获取文件内容
        std::string body;
        fu.GetContent(body);
        cout<<"GetContent success!"<<endl;
        rep.body=body;
        rep.content_length_=body.size();
        if(flag==false){
        //不需要断点续传
         rep.status=200;
         rep.set_header("Accept-Ranges","bytes");
         rep.set_header("Content-Type","application/octet-stream");
         rep.set_header("Etag",GetEtag(fileinfo.back_path));
        }
        else{
          //断点续传
          rep.status=206;
          rep.set_header("Accept-Ranges","bytes");
          rep.set_header("Content-Type","application/octet-stream");
          rep.set_header("ETag",GetEtag(fileinfo.back_path));
        }
        return ;
      }
    public:
      void Run(){
        server.Post("/upload",UpLoad); 
        server.Get("/download/(.*)",DownLoad);
        server.Get("/listshow",ListShow);
        server.Get("/",ListShow);
        server.listen("0.0.0.0",port);//启动服务器
      }
      static Service* GetInstant(){
        if(instant==nullptr){
          lock.lock();
          if(instant==nullptr){
            instant=new Service();
          }
          lock.unlock();
        }
        return instant;
      }
  };

  std::mutex Service::lock;
  Service* Service::instant=nullptr;
}
