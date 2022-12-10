
#pragma once
#include<iostream>
#include<vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<string>
#include<fstream>
#include</usr/include/jsoncpp/json/json.h> 
#include"./bundle.h"
#include<memory>
#include<sstream>
#include<experimental/filesystem>
using std::endl;
using std::cout;

namespace fs = std::experimental::filesystem;
namespace sjp{
  //文件工具
  //获取一个文件属性信息:
  //大小，最近访问时间，最近修改时间
  //判断文件是否存在,文件名
  //往文件中写入数据，读取数据
  //删除文件
  //获取目录中的相对路径
  //压缩文件
  //解压缩文件
  //如果是目录，获取目录下所有文件的相对路径
  class FileUtil{
    private:
      std::string filename;
    public:
      FileUtil(const char* str){
        filename=str;
      }

      FileUtil(const std::string s){
        filename=s;
      }
      size_t GetFileSize();       //获取文件大小
      size_t GetFileAccessTime() ;//获取文件最后一次访问时间
      size_t GetFileModfityTime();//获取文件最后一次修改时间
      std::string GetFilename();  //获取文件名
      bool Exist();               //判断文件是否存在
      bool SetContent(std::string& body);//往文件中写入body的内容
      bool GetContent(std::string& body);//获取文件内容，将文件内容放入body中
      //获取文件的部分内容
      bool GetPosContent(size_t pos,size_t n,std::string& body);
      bool Remove();//删除文件
      //压缩文件，压缩文件名为packname
      bool PackFile(const std::string&packname);
      //解压缩文件，解压缩文件名为packname
      bool UnpackFile(const std::string& unpackname);
      //获取目录下的所有文件的相对路径
      bool GetPathInDir(std::vector<std::string>& arry);
      //创建目录
      bool CreateDir(std::string& dirname){
        fs::create_directory(dirname);
        return true;
      }
  };

  class JsonUtil
  {
  public:
    //将root序列化为Json字符串
   static bool Serialize(Json::Value& root,std::string& body)    
    {    
      Json::StreamWriterBuilder swb;        
      std::unique_ptr<Json::StreamWriter> s(swb.newStreamWriter());        
      std::stringstream ss;        
      //将value对象转换为Json数据流        
      s->write(root,&ss);      
      body=ss.str();    
      return true;    
    }    
      
  //将Json字符串反序列化为Json::Value
  static bool UnSerialize(Json::Value& root,std::string& body)    
  {    
    Json::CharReaderBuilder crb;    
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());    
    std::string err;    
    bool ret=cr->parse(body.c_str(),body.c_str()+body.size(),&root,&err);    
    return true;
  }    
  };
}


