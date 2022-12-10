#include"hotspot.hpp"
#include"service.hpp"
using namespace std;
void FileUtil_test(const char* filename)
{
  sjp::FileUtil file(filename);
//  cout<<file.GetFileSize()<<endl;
//  cout<<file.GetFileAccessTime()<<endl;
//  cout<<file.GetFileModfityTime()<<endl;
//  cout<<file.GetFilename()<<endl;
//   std::string body;
//  file.GetContent(body);
//  cout<<body<<endl;
// sjp::FileUtil file1("test.txt");
//  file1.Remove();
 /* std::string packfile=filename;
  packfile+=".lz";
  file.PackFile(packfile);

  sjp::FileUtil file2(packfile.c_str());
  file2.UnpackFile("test1.txt");

  */

  Json::Value root;
  std::string body;
  std::string s=R"({"姓名":"小黑","年龄":19,"成绩":[43,53,45]})";
  sjp::JsonUtil::UnSerialize(root,s);
  sjp::JsonUtil::Serialize(root,body);
  file.SetContent(body);
  cout<<body<<endl;
}

/*
void Config_test()
{
  sjp::Config cf;
  cout<<"backdir: "<<cf.back_dir<<endl;
  cout<<"hottime: "<<cf.hot_time<<endl;
  cout<<"ip: "<<cf.ip<<endl;
  cout<<"packdir: "<<cf.pack_dir<<endl;

  cout<<"serverport: "<<cf.server_port<<endl;
}
*/

void FileInfoManger_test(){
  sjp::FileInfo info;
  info.access_time=100;
  info.back_path="./main.cc";
  info.url="./wwwroot/main.cc";
  sjp::FileInfoManger::GetInstant()->Insert(info); 
  sjp::FileInfo info1;
  info1.access_time=200;
  info1.back_path="./main.cc";
  info1.url="./wwwroot/main";
  sjp::FileInfoManger::GetInstant()->Insert(info1); 
  sjp::FileInfoManger::GetInstant()->Storage(); 
}

void FileInfoManger_test1()
{
  std::vector<sjp::FileInfo> arry;
  sjp::FileInfoManger::GetInstant()->GetAllInfo(arry);
  cout<<arry.size()<<endl;
  cout<<arry[0].url<<endl;
  cout<<arry[1].url<<endl;
}

void Config_test()
{
  cout<<sjp::Config::GetInstant()->GetBackFileInfo()<<endl;
  cout<<sjp::Config::GetInstant()->GetBackDir()<<endl;
  cout<<sjp::Config::GetInstant()->GetPackDir()<<endl;
  cout<<sjp::Config::GetInstant()->GetHotTime()<<endl;
  cout<<sjp::Config::GetInstant()->GetIp()<<endl;
  cout<<sjp::Config::GetInstant()->GetUrlPrefix()<<endl;
}

void FileUtil1_test()
{
  sjp::FileUtil fu("wwwroot/");
  std::vector<std::string> arry;
  fu.GetPathInDir(arry);
  for(auto it:arry){
    cout<<it<<endl;
  }
  return ;
}


void HotSpotsManger_test(){
  sjp::HotSpotManger hsm;
  hsm.Run();
}

void FileUtil_test1(){
  sjp::FileUtil fu("./wwwroot");
  vector<std::string> arry;
  fu.GetPathInDir(arry); 
  for(int i=0;i<arry.size();i++){
    cout<<arry[i]<<endl;
  }
}
void FileInfoManger_test2(){
  sjp::FileInfo info;
  info.access_time=100;
  info.url="1111";
  sjp::FileInfoManger::GetInstant()->Insert(info);
}

void service_test(){
   sjp::Service s;
   s.Run();
}

//测试文件信息管理系统在启动的是否更新文件信息
void FileInfoManger_test3(){
  std::vector<sjp::FileInfo> arry;
  sjp::FileInfoManger::GetInstant()->InitLoad();
//  for(int i=0;i<arry.size();i++){
//    cout<<arry[i].back_path<<endl;
//  }
//  //>
}

int main(int argc,char* argv[])
{
//  if(argc!=2){
//    cout<<"./main filename"<<endl;
//    return -1;
//  }
  //FileInfoManger_test1();
  // Config_test();
  //FileUtil_test("test.txt");
  //FileUtil1_test();
  //HotSpotsManger_test();
  //FileInfoManger_test2();
  //FileUtil_test1();
  service_test();
  //FileInfoManger_test3();
  return 0;
}
