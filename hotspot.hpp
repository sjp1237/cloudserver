#pragma once
#include"FileInfoManger.hpp"
/*
    对服务器上的备份文件进行检测，哪些文件长时间没有被访问,
    则认为为非热点文件压缩存储

    实现思路：
    1.遍历所有的文件，检测文件最后一次的访问时间,与当前时间进行相减，这个差值
    如果大于设定号的非热点判断时间则认为是非热点文件
    2.如果是非热点文件，压缩原文件则需要删除原文件，节省空间
 */
namespace sjp{
  class HotSpotManger{
    private:
      int hot_time;
      std::string back_dir;//压缩文件目录;
      std::string pack_dir;//备份文件目录;
      std::string pack_file_suffix;
      static std::mutex lock;
      static HotSpotManger* instant;
      HotSpotManger(){
        hot_time=sjp::Config::GetInstant()->GetHotTime();
        back_dir=sjp::Config::GetInstant()->GetBackDir();
        pack_dir=sjp::Config::GetInstant()->GetPackDir();
        pack_file_suffix=sjp::Config::GetInstant()->GetPackFileSuffix();
      }

      HotSpotManger(HotSpotManger&)=delete;
    public:
      //热点文件返回true，非热点文件返沪false 
      static HotSpotManger* GetInstant(){
        if(instant==nullptr){
          lock.lock();
          if(instant==nullptr){
            instant=new HotSpotManger();
          }
          lock.unlock();
        }
        return instant;
      }


      //热点文件返回true
      //非热点文件返回false
      bool JudgeHotspotFile(std::string& backpath){
        sjp::FileUtil fu(backpath);
        time_t atime=fu.GetFileAccessTime();
        time_t curtime=time(NULL);
        if(curtime-atime>hot_time){
          return false;
        }
        return true;
      }

      void Run(){
        sjp::FileUtil fu(back_dir.c_str());
        if(!fu.Exist())
        {
           fu.CreateDir(back_dir); 
        }

        while(true){
        std::vector<std::string> arry;
        fu.GetPathInDir(arry);
        for(int i=0;i<arry.size();i++){
          //获取文件信息
          sjp::FileInfo fileinfo;
          if(!sjp::FileInfoManger::GetInstant()->GetOneByRealPath(arry[i],fileinfo))
          {
            //存储文件中没有该文件信息
            fileinfo.AddFileInfo(arry[i]);
            sjp::FileInfoManger::GetInstant()->Insert(fileinfo);
          }
          if(!JudgeHotspotFile(fileinfo.back_path)){
            /*1.非热点文件进行压缩
             *2.删除源文件
              3.修改文件列表中的信息
             */
            sjp::FileUtil fu(arry[i]);
            std::string packfilepath=pack_dir+fu.GetFilename()+pack_file_suffix; 
            fu.PackFile(packfilepath);
            fu.Remove();
            fileinfo.pack_sign=true; 
            sjp::FileInfoManger::GetInstant()->update(fileinfo);
          }
        }
        sjp::FileInfoManger::GetInstant()->Storage();
        usleep(1000);
      }
      }
  };

  sjp::HotSpotManger* HotSpotManger::instant=nullptr;
  std::mutex HotSpotManger::lock;
}
