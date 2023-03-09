#include <string.h>
#include "csv.h"
#include "ini.h"
#include "file.h"

typedef struct
{
	const char *filePathIn;
	const char *fllePathOut;
}ConfigInfo_T;

struct DataHandle
{
	int col;
	int row;
	char word[128];
	ConfigInfo_T *configInfo_t;
};

size_t word_to_string(const char *data, void *str_buf)
{
	return sprintf(str_buf, "%s\n", data);
}

size_t csv_write_file(FILE *fp, void *data, size_t size)
{
	size_t count = 0;
	char *data_p = data;
	if (fp == NULL)
	{
		return count;
	}
	for (size_t i = 0; i < size; i++)
	{
		if (fputc(data_p[i], fp) == EOF)
		{
			return count;
		}
		++count;
	}
	return count;
}

int csv_write_word2file(const char *file_path, const char *data)
{
	char buf[1024];
	FILE *fp;
	size_t len, res = 0;
	len = word_to_string(data, buf);
	if (len == 0)
	{
		return -1;
	}
	fp = fopen(file_path, "ab");
	do
	{
		res = csv_write_file(fp, buf, len);
		len -= res;
	}
	while (len != 0);
	fclose(fp);
	return 0;
}

static int is_english_word(const char buf[], int len)
{
	int charCount = 0;
	int result = 0;
	for (int i = 0; i < len; i++)
	{
		if (('A' <= buf[i] && buf[i] <= 'Z') || ('a' <= buf[i] && buf[i] <= 'z'))
		{
			charCount++;
		}
	}
	if (charCount == len)
	{
		result =  0;
	}
	else
	{
		result = -1;
	}
	return result;
}

// 列级数据处理回调函数（先处理）
void col_callback(void *s, size_t len, void *data)
{
	struct DataHandle *handle = (struct DataHandle *)data;
	char *str = malloc(len + 1);
	memset(str, 0, len + 1);
	memcpy(str, (char *)s, len);
	if (1 == handle->col) //只处理第二列
	{
		if (0 == is_english_word(str, len))
		{
			sprintf(handle->word, "%s", str);
		}
	}
	free(str);
	// 每进入该函数一次，列计数 + 1
	(handle->col)++;
}

// 行级数据处理回调函数（后处理）
void row_callback(int c, void *data)
{
	struct DataHandle *handle = (struct DataHandle *)data;
	
	if(strlen(handle->word) > 0)
	{
		csv_write_word2file(handle->configInfo_t->fllePathOut, handle->word);
		memset(handle->word, 0, sizeof(handle->word));
	}
	// 每进入该函数一次，行计数 + 1，列计数 清零
	(handle->row)++;
	//printf("row:%d col:%d\n", handle->row, handle->col);
	handle->col = 0;
}

int csv_parse_file(const char *file_path, void(*cols_callback)(void *, size_t, void *),
	void(*rows_callback)(int c, void *), void *data)
{
	struct csv_parser p;
	FILE *fp = NULL;
	size_t bytes_read = 0;
	size_t retval = 0;
	char buf[1024] = { 0 };
	int result = -1;

	if (csv_init(&p, CSV_STRICT | CSV_STRICT_FINI) != 0) // 初始化
	{
		printf("failed to initialize csv parser\n");
		return result;
	}

	fp = fopen(file_path, "rb"); // 打开文件
	if (fp == NULL)
	{
		fprintf(stderr, "Failed to open file %s: %s\n", file_path, strerror(errno));
		goto END;
	}
	while ((bytes_read = fread(buf, sizeof(char), 1024, fp)) > 0)
	{
		retval = csv_parse(&p, buf, bytes_read, cols_callback, rows_callback, data);
		if (retval != bytes_read)
		{
			fprintf(stderr, "Error parsing file: %s\n", csv_strerror(csv_error(&p)));
			goto END;
		}
	}

	if (csv_fini(&p, cols_callback, rows_callback, data) != 0)
	{
		goto END;
	}
	result = 0;
END:
	csv_free(&p);
	fclose(fp);
	return result;
}

static int handler(void* user, const char* section, const char* name,
	const char* value)
{
	ConfigInfo_T* pConfig = (ConfigInfo_T*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (MATCH("Configuration", "file_path_in"))
	{
		pConfig->filePathIn = strdup(value);
	}
	else
	{
		return 0;  /* unknown section/name, error */
	}
	return 1;
}

int main(int argc, const char *argv[])
{
	// Loading ini configuration information
	ConfigInfo_T configInfo_t = {0};
	if (ini_parse("config.ini", handler, &configInfo_t) < 0)
	{
		printf("Can't load 'config.ini'\n");
		return -1;
	}
	printf("Config loaded from 'config.ini': file_path_in=%s\n\n", configInfo_t.filePathIn);

	// Deleting the output file eq 1.txt
	char driveName[300] = { 0 };
	char dirName[300] = { 0 };
	char fileName[300] = { 0 };
	SplitPath(configInfo_t.filePathIn, driveName, dirName, fileName);
	char filePathOut[300] = {0};
	char searchDir[300] = { 0 };
	sprintf(searchDir, "%s%s", driveName, dirName);
	sprintf(filePathOut, "%s\\1.txt", searchDir);
	configInfo_t.fllePathOut = filePathOut;
	char fileList[50][300] = { 0 };
	int numOfFile = 0;
	GetCurrentAllFile(searchDir, fileList, &numOfFile);
	for (int i = 0; i < numOfFile; i++)
	{
		//printf("%s\n", fileList[i]);
		if (0 == strcmp(filePathOut, fileList[i]))
		{
			remove(filePathOut);
		}
	}

	// Parsing csv file
	struct DataHandle handle = { 0 };
	handle.configInfo_t = &configInfo_t;
	csv_parse_file(configInfo_t.filePathIn, col_callback, row_callback, &handle);
	printf("Parse Row:%d\n", handle.row);
	return 0;
}