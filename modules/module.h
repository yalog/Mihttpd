/*
* 	Mihttpd/modules/module.h
*
*	(c) 2011  yalog 
*/
/*
*	����ļ���Ҫ����ע��http��չ����ģ��
*/
#include ""

struct module {
	void (* handler)(request_t *); //ģ��Ĵ�����ع���
};

//����ע��ģ����ر�
struct module modules[] = {
	&handler_cgi,
	NULL
};