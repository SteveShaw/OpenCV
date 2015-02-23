#include <QCoreApplication>
#include <stdio.h>
#include "ftpread.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

//#define LOCAL_FILE      "/tmp/uploadthis.txt"
//#define UPLOAD_FILE_AS  "while-uploading.txt"
//#define REMOTE_URL      "ftp://example.com/"  UPLOAD_FILE_AS
//#define RENAME_FILE_TO  "renamed-and-fine.txt"

	QByteArray LOCAL_FILE("E:/Video/Depth.m4v");
	QByteArray UPLOAD_FILE_AS("DepthCurl.mp4");
	QByteArray REMOTE_URL("ftp://129.105.36.214/");
	REMOTE_URL+=UPLOAD_FILE_AS;
	QByteArray RENAME_FILE_TO("RenamedDepthCurl.mp4");

	CURL *curl;
	CURLcode res;
//	FILE *hd_src;
//	struct stat file_info;
	curl_off_t fsize;

	struct curl_slist *headerlist=NULL;
//	static const char buf_1 [] = "RNFR ";// UPLOAD_FILE_AS;
//	static const char buf_2 [] = "RNTO ";// RENAME_FILE_TO;

	QByteArray cmd_1 = "RNFR ";
	cmd_1 += UPLOAD_FILE_AS;
	QByteArray cmd_2 = "RNTO ";
	cmd_2 += RENAME_FILE_TO;
	QByteArray cmd_3 = "TYPE BINARY";

	QFile f(LOCAL_FILE.data());

	fsize = f.size();

	if(fsize==0)
	{
		qDebug()<<"Error in this file";
		return 1;
	}


//	/* get the file size of the local file */
//	if(stat(LOCAL_FILE.data(), &file_info)) {
//		printf("Couldnt open '%s': %s\n", LOCAL_FILE, strerror(errno));
//		return 1;
//	}
//	fsize = (curl_off_t)file_info.st_size;

//	printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

	/* get a FILE * of the same file */
//	hd_src = fopen(LOCAL_FILE, "rb");

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();

	if(curl) {

		f.open(QIODevice::ReadOnly);
		/* build a list of commands to pass to libcurl */
		headerlist = curl_slist_append(headerlist,cmd_3.data());
		headerlist = curl_slist_append(headerlist, cmd_1.data());
		headerlist = curl_slist_append(headerlist, cmd_2.data());

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, FTPRead::read_callback);

		/* enable uploading */
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* specify target */
		curl_easy_setopt(curl,CURLOPT_URL, REMOTE_URL.data());

		/* pass in that last of FTP commands to run after the transfer */
		curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

		/* now specify which file to upload */
		curl_easy_setopt(curl, CURLOPT_READDATA, &f);

		 curl_easy_setopt(curl, CURLOPT_USERPWD, "jianshao:111111");

		/* Set the size of the file to upload (optional).  If you give a *_LARGE
			 option you MUST make sure that the type of the passed-in argument is a
			 curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
			 make sure that to pass in a type 'long' argument. */
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
										 (curl_off_t)fsize);

		/* Now run off and do what you've been told! */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
							curl_easy_strerror(res));

		/* clean up the FTP commands list */
		curl_slist_free_all (headerlist);

		/* always cleanup */
		curl_easy_cleanup(curl);

		f.close();
	}
//	fclose(hd_src); /* close the local file */

	curl_global_cleanup();
	return 0;

	return a.exec();
}
