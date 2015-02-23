#ifndef FTPREAD_H
#define FTPREAD_H
//#include <QFile>
#include <QDataStream>
#include <curl/curl.h>

class FTPRead
{
public:
	FTPRead();
	virtual ~FTPRead();

	static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		curl_off_t nread;
		/* in real-world cases, this would probably get this data differently
			 as this fread() stuff is exactly what the library already would do
			 by default internally */
		QIODevice* io = static_cast<QIODevice*>(stream);
		nread = io->read((char*)ptr,size*nmemb);
//		size_t retcode = fread(ptr, size, nmemb, static_cast<FILE*>(stream));

//		nread = (curl_off_t)retcode;

		fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
						" bytes from file\n", nread);
		return nread;
	}
};

#endif // FTPREAD_H
