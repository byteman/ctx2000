#include <stdio.h>
#include <stdlib.h>
#include <string>
bool exportdata(std::string tc_serial, std::string path)
{
	char buf[128]={0,};
	snprintf(buf,128,"./dump.sh %s %s",tc_serial.c_str(),path.c_str());
	int ret = system(buf);	
	printf("cmd = %s ret = %d\n",buf,ret);
	return !ret; 
}
int main()
{
	exportdata("tc104","/dev");
	return 0;
}
