#pragma once
#include"Config.hpp"
#include<unordered_map>
#include<pthread.h>
namespace sjp{
  class FileInfo{
    public:
      bool pack_sign;
      size_t file_size;
      time_t access_time;
      time_t modify_time;
      std::string back_path;
      std::string pack_path;
      std::string url;
    public:
      FileInfo(const std::string& backpath){
        AddFileInfo(backpath);
      }
      FileInfo(){}
      bool AddFileInfo(const std::string& backpath){
        sjp::FileUtil fu(backpath.c_str());
        file_size=fu.GetFileSize();
        pack_sign=false;
        access_time=fu.GetFileAccessTime();
        modify_time=fu.GetFileModfityTime();
        back_path=sjp::Config::GetInstant()->GetBackDir();
        back_path+=fu.GetFilename();
        pack_path=sjp::Config::GetInstant()->GetPackDir();
        pack_path+=fu.GetFilename();
        pack_path+=sjp::Config::GetInstant()->GetPackFileSuffix();
        url="/download/";
        url+=fu.GetFilename();
        return true;
      }
  };

  class FileInfoManger{
    private:
      std::string file_info_path;//存储文件信息的持久化文件
      std::unordered_map<std::string,FileInfo> _table; //backpath-info
      pthread_rwlock_t _rwlock;//读写锁
    private:
      static FileInfoManger* instant;
      static std::mutex lock;
    private:
      FileInfoManger(){
        file_info_path=Config::GetInstant()->GetBackFileInfo();
        pthread_rwlock_init(&_rwlock,NULL);
        InitLoad(); 
        UpdatePackSign();
      }

      FileInfoManger(FileInfoManger&)=delete;
    public:
      bool UpdateFileInfo(){
        std::string path=sjp::Config::GetInstant()->GetBackDir();
        sjp::FileUtil fu(path);
        std::vector<std::string> arry;
        fu.GetPathInDir(arry);

        for(int i=0;i<arry.size();i++){
          auto it=_table.find(arry[i]);
          if(it==_table.end()){
            //>
            //没有将该文件信息存储到信息系统中
            sjp::FileInfo fileinfo(arry[i]); 
            Insert(fileinfo);
          }
        }
        Storage();
      }


      bool UpdatePackSign(){
        cout<<"Updatepacksign"<<endl;
        auto it=_table.begin();
        while(it!=_table.end()){
          sjp::FileUtil fu(it->second.pack_path);
          if(fu.Exist()){
            it->second.pack_sign=true;
            Insert(it->second);
          }
          it++;
        }
        Storage();
      }

      static FileInfoManger* GetInstant(){    
          if(instant==nullptr){    
            lock.lock();    
            if(instant==nullptr){    
              instant=new FileInfoManger();    
            }    
            lock.unlock();    
          }    
          return instant;    
        }              
      //插入信息
      bool Insert(const FileInfo& info){
         pthread_rwlock_wrlock(&_rwlock);
         _table[info.back_path]=info;
         pthread_rwlock_unlock(&_rwlock);
         return true;
      }
      //更新信息
      bool update(const FileInfo& info){
         pthread_rwlock_wrlock(&_rwlock);
         _table[info.back_path]=info;
         pthread_rwlock_unlock(&_rwlock);
         return true;
      }
      //通过backpath判断该文件是否存在
      bool Exist(const std::string& backpath)
      {
        auto it=_table.find(backpath);
        if(it!=_table.end()){
          return true;
        }
        return false;
      }

      //通过url获取文件信息
      bool GetoneByURL(const std::string& url,FileInfo& info){
        sjp::FileUtil fu(url);        
        std::string backpath=sjp::Config::GetInstant()->GetBackDir();
        backpath+=fu.GetFilename();
        return  GetOneByRealPath(backpath,info);
      }

      //通过相对路径获取文件信息
      bool GetOneByRealPath(const std::string& backpath,FileInfo& info){
        auto it=_table.find(backpath);
        if(it!=_table.end()){
          info=it->second;
          return true;
        }
         return false;
      }

      //获取所有文件的属性信息
      bool GetAllInfo( std::vector<FileInfo>& arry){
        pthread_rwlock_wrlock(&_rwlock);
        auto it=_table.begin();
        while(it!=_table.end())
        {
          arry.push_back(it->second);
          it++;
        }
        pthread_rwlock_unlock(&_rwlock);
        return true;
      }
      //将table上的所有信息存储持久化到文件上
      bool Storage(){
        //将文件的备份信息反序列化字符并组织到body上
        std::string filebody;
          Json::Value root; 
         std::string body;
         auto it=_table.begin();
         int i=0;
        while(it!=_table.end())
        {
         root[i]["AccessTime"]=(Json::Int)it->second.access_time;    
         root[i]["ModifyTime"]=(Json::Int)it->second.modify_time;    
         root[i]["FileSize"]=(Json::Int)it->second.file_size;    
         root[i]["Url"]=it->second.url;    
         root[i]["PackSign"]=it->second.pack_sign;    
         root[i]["BackPath"]=it->second.back_path;    
         root[i]["PackPath"]=it->second.pack_path;
         i++;
         it++;
        }
         sjp::JsonUtil::Serialize(root,body);
        sjp::FileUtil fu(file_info_path.c_str());
        if(!fu.SetContent(body))
        {
          return false;
        }
        return true;
      }


      //将文件上的信息加载到内存中
      bool InitLoad(){
        /*
         * 1.打开备份文件的存储信息
           2.将存储信息读取到body中
           3.将body进行反序列化为root;
           4.将各个文件信息加载到table中
        */
        sjp::FileUtil fu(file_info_path.c_str()); 
        std::string body;
        fu.GetContent(body);
        Json::Value root;
        sjp::JsonUtil::UnSerialize(root,body);
        for(int i=0;i<root.size();i++){
          FileInfo fileinfo;
          fileinfo.access_time=root[i]["AccessTime"].asInt();
          fileinfo.modify_time=root[i]["ModifyTime"].asInt();
          fileinfo.back_path=root[i]["BackPath"].asString();
          fileinfo.pack_path=root[i]["PackPath"].asString();
          fileinfo.file_size=root[i]["FileSize"].asInt();
          fileinfo.pack_sign=root[i]["PackSign"].asBool();
          fileinfo.url=root[i]["Url"].asString();
          Insert(fileinfo);
        }
        UpdateFileInfo();
        return true;
      }

      ~FileInfoManger(){
        Storage();
      }
  };
  FileInfoManger* FileInfoManger::instant=nullptr;
  std::mutex FileInfoManger::lock;
}
