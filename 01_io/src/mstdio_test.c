#include "mstdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main1(void)
{
	MFILE *fp1 = mfopen("/etc/passwd", "r");
	assert(fp1 != NULL);
	
	MFILE *fp2 = mfopen("/home/denghengli/winshare/systemProgram/io/test/mypasswd", "w");
	assert(fp2 != NULL);
	
	char c;
	while((c = mfgetc(fp1)) != MEOF) {
		mfputc(c, fp2);
	}
	
	mfclose(fp1);
	mfclose(fp2);
	
	return 0;
}

int main(int argc, char *argv[])
{
	char str[] = "mstdio mfwrite test\n";
	char rbuf[1024] = {'\0'};
	MFILE *fp;
	
	if(argc < 2){
		printf("usage: %s mfwrite or mfread\n", argv[0]);
		exit(1);
	}
	
	if(!strcmp(argv[1], "mfwrite")) 
	{
		fp = mfopen("/home/denghengli/winshare/systemProgram/io/test/mstdio_test.txt", "w");
		assert(fp != NULL);
		mfwrite(str, sizeof(str), 1, fp);
	} 
	else if(!strcmp(argv[1], "mfread")) 
	{
		fp = mfopen("/home/denghengli/winshare/systemProgram/io/test/mstdio_test.txt", "r");
		assert(fp != NULL);
		mfread(rbuf, sizeof(rbuf), 1, fp);
		printf("read data is: %s", rbuf);
	}
	
	mfclose(fp);
	
	return 0;
}

