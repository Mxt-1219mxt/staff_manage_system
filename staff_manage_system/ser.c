#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sqlite3.h>
#include <time.h>
#include "staff_manage_system.h"

//需要传入到分支线程的数据类型;
struct msg
{
	int newfd;
	struct sockaddr_in cin;
};
sqlite3 *db = NULL;

void * pthread_ser(void* arg)
{
    //分离当前线程，退出后自动回收资源
    pthread_detach(pthread_self());

    struct msg cliInfo = *(struct msg *)arg;
    int newfd = cliInfo.newfd;
    struct sockaddr_in cin = cliInfo.cin;

    struct my_user rev_myuser;

    char buf[128] = "";
    ssize_t res = 0;
    while(1)
    {
        res = recv(newfd, &rev_myuser, sizeof(rev_myuser), 0);
        if(res < 0)
        {
            ERR_MSG("recv");
            return NULL;
        }
        switch(rev_myuser.num)
        {
            case 0:
                ser_entry(newfd, &rev_myuser, &cin);
                break;
            case 1:
                ser_select(newfd, &rev_myuser, &cin);
                break;
            case 2:
                ser_modify(newfd, &rev_myuser, &cin);
                break;
            case 3:
                ser_add(newfd, &rev_myuser, &cin);
                break;
            case 4:
                ser_del(newfd, &rev_myuser, &cin);
                break;
            case 5:
                ser_select_history(newfd, &rev_myuser, &cin);
                break;
            case 10:
                ser_entry(newfd, &rev_myuser, &cin);
                break;
            case 111:
                ser_select(newfd, &rev_myuser, &cin);
                break;
            case 12:
                ser_modify(newfd, &rev_myuser, &cin);
                break;
        }
        if(res == 0)
        {
            printf("[%s:%d]newfd=%d 客户端关闭\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);      
            pthread_exit(NULL);
        }
    }

    return NULL;
}

//用户登录
void ser_entry(int sfd, void * arg, void *arg1)
{
    struct sockaddr_in cin = *((struct sockaddr_in*)arg1);
    struct my_user myuser = *((struct my_user*)arg);
    char buf[10] = "";
    char sql[128] = "";
	char *errmsg = NULL;
	char **result = NULL;
	int row,column;
	sprintf(sql,"select passwd from staff where type='%s' and name='%s'",myuser.type, myuser.username);
	printf("%s\n",sql);
    if(sqlite3_get_table(db, sql, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return;
	}
    if(!strcmp(myuser.userpswd, result[1]))
    {
        strcpy(buf, "success");
        printf("%s\n",buf);
        if(send(sfd, &buf, sizeof(buf), 0) < 0)
        {
            ERR_MSG("send");
            return;
        }
    }
    else
    {
        strcpy(buf, "failed");
        printf("%s\n",buf);
        if(send(sfd, &buf, sizeof(buf), 0) < 0)
        {
            ERR_MSG("send");
            return;
        }
    }
    sqlite3_free_table(result);
}
//用户查询
void ser_select(int sfd, void * arg, void *arg1)
{
    struct sockaddr_in cin = *((struct sockaddr_in*)arg1);
    struct my_user myuser = *((struct my_user*)arg);
    struct my_user recv_user;
    struct my_staff send_mystaff;
    char buf[128] = "";
    char type[64] = "";
    char sql[256] = "";
	char *errmsg = NULL;
	char **result = NULL;
	int row,column;

    if(recv(sfd, &recv_user, sizeof(recv_user), 0) < 0)
    {
        ERR_MSG("recv");
    }
    if(myuser.num == 111)
    {
        sprintf(sql,"select * from staff where name='%s'",myuser.username);
        printf("%s\n",sql);
        if(sqlite3_get_table(db, sql, &result, &row, &column, &errmsg) != SQLITE_OK)
        {
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
            return;
        }
        strcpy(send_mystaff.id, result[11]);
        strcpy(send_mystaff.type, result[12]);
        strcpy(send_mystaff.name, result[13]);
        strcpy(send_mystaff.passwd, result[14]); 
        strcpy(send_mystaff.age, result[15]);
        strcpy(send_mystaff.phone, result[16]);
        strcpy(send_mystaff.address, result[17]);
        strcpy(send_mystaff.post, result[18]);
        strcpy(send_mystaff.time, result[19]); 
        strcpy(send_mystaff.score, result[20]);
        strcpy(send_mystaff.wage, result[21]);

        if(send(sfd, &send_mystaff, sizeof(send_mystaff), 0) < 0)
        {
            ERR_MSG("send");
        }
    }
    else if(recv_user.num == 101)
    {
        sprintf(sql,"select * from staff where name='%s'",recv_user.arr);
        printf("%s\n",sql);
        if(sqlite3_get_table(db, sql, &result, &row, &column, &errmsg) != SQLITE_OK)
        {
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
            return;
        } 
        printf("打开表格成功\n");
        if(result[11] == NULL)
        {
            if(send(sfd, "failed", sizeof("failed"), 0) < 0)
            {
                ERR_MSG("send");
            }
        }
        strcpy(send_mystaff.id, result[11]);
        strcpy(send_mystaff.type, result[12]);
        strcpy(send_mystaff.name, result[13]);
        strcpy(send_mystaff.passwd, result[14]); 
        strcpy(send_mystaff.age, result[15]);
        strcpy(send_mystaff.phone, result[16]);
        strcpy(send_mystaff.address, result[17]);
        strcpy(send_mystaff.post, result[18]);
        strcpy(send_mystaff.time, result[19]); 
        strcpy(send_mystaff.score, result[20]);
        strcpy(send_mystaff.wage, result[21]);

        if(send(sfd, &send_mystaff, sizeof(send_mystaff), 0) < 0)
        {
            ERR_MSG("send");
        }       
    }
    else if(recv_user.num == 102)
    {
        sprintf(sql,"select * from staff");
        printf("%s\n",sql);
        if(sqlite3_get_table(db, sql, &result, &row, &column, &errmsg) != SQLITE_OK)
        {
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
            return;
        }
        int i = 11;
        send_mystaff.eflag = 1;
        while(i < ((row+1)*column))
        {
            strcpy(send_mystaff.id, result[i++]);
            strcpy(send_mystaff.type, result[i++]);
            strcpy(send_mystaff.name, result[i++]);
            strcpy(send_mystaff.passwd, result[i++]); 
            strcpy(send_mystaff.age, result[i++]);
            strcpy(send_mystaff.phone, result[i++]);
            strcpy(send_mystaff.address, result[i++]);
            strcpy(send_mystaff.post, result[i++]);
            strcpy(send_mystaff.time, result[i++]); 
            strcpy(send_mystaff.score, result[i++]);
            strcpy(send_mystaff.wage, result[i++]);

            if(send(sfd, &send_mystaff, sizeof(send_mystaff), 0) < 0)
            {
                ERR_MSG("send");
            }
        }
        send_mystaff.eflag = 0;
                if(send(sfd, &send_mystaff, sizeof(send_mystaff), 0) < 0)
            {
                ERR_MSG("send");
            }
    }
    sqlite3_free_table(result);
}
//修改信息
void ser_modify(int sfd, void * arg, void *arg1)
{
    struct sockaddr_in cin = *((struct sockaddr_in*)arg1);
    struct my_user myuser = *((struct my_user*)arg);
    char buf[188] = "";
    char mytime[64] = "";
    char arr[20] = "";
    char arr1[20] = "";
    char brr[20] = "";
    char sql[256] = "";
	char *errmsg = NULL;
	char **result = NULL;
	int row,column;
    bzero(buf, sizeof(buf));
    if(recv(sfd, &myuser, sizeof(myuser), 0) < 0)
    {
        ERR_MSG("recv");
    }

	sprintf(sql,"select id from staff where type='%s' and name='%s'",myuser.type, myuser.username);
	printf("%s\n",sql);
    if(sqlite3_get_table(db, sql, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return;
	}

    if(!strcmp(myuser.type, "0"))
    {
        strcpy(arr, "管理员");
    }
    else if(!strcmp(myuser.type, "1"))
    {
        strcpy(arr, "普通用户");
    }
    
    switch(myuser.num)
    {
        case 21:
            strcpy(brr, "name");
            sprintf(sql,"update staff set name='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 22:
            strcpy(brr, "age");
            sprintf(sql,"update staff set age='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 23:
            strcpy(brr, "address");
            sprintf(sql,"update staff set address='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 24:
            strcpy(brr, "phone");
            sprintf(sql,"update staff set phone='%s' where id='%s'", myuser.arr, myuser.id);
            break; 
        case 25:
            strcpy(brr, "post");
            sprintf(sql,"update staff set post='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 26:
            strcpy(brr, "wage");
            sprintf(sql,"update staff set wage='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 27:
            strcpy(brr, "time");
            sprintf(sql,"update staff set time='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 28:
            strcpy(brr, "score");
            sprintf(sql,"update staff set score='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 29:
            strcpy(brr, "passwd");
            sprintf(sql,"update staff set passwd='%s' where id='%s'", myuser.arr, myuser.id);
            break;
        case 121:
            strcpy(brr, "name");
            sprintf(sql,"update staff set name='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 122:
            strcpy(brr, "age");    
            sprintf(sql,"update staff set age='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 123:
            strcpy(brr, "address");
            sprintf(sql,"update staff set address='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 124:
            strcpy(brr, "phone");
            sprintf(sql,"update staff set phone='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 125:
            strcpy(brr, "post");
            sprintf(sql,"update staff set post='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 126:
            strcpy(brr, "wage");
            sprintf(sql,"update staff set wage='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 127:
            strcpy(brr, "time");
            sprintf(sql,"update staff set time='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 128:
            strcpy(brr, "score");
            sprintf(sql,"update staff set score='%s' where id='%s'", myuser.arr, result[1]);
            break;
        case 129:
            strcpy(brr, "passwd");
            sprintf(sql,"update staff set passwd='%s' where id='%s'", myuser.arr, result[1]);
            break;
    }
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
    }
    strcpy(arr1, "success");
    buf[strlen(arr1)] = '\0';
    printf("%s\n",arr1);
    if(send(sfd, arr1, sizeof(arr1), 0) < 0)
    {
        ERR_MSG("send");
    }
    printf("id=%s,name=%s,arr=%s\n",result[1],brr,myuser.arr);
    sprintf(buf, "---%s---%s%s修改工号为%s的%s为%s",myuser.username, arr,myuser.username,myuser.id, brr,myuser.arr);
    struct tm* info = NULL;
    time_t t;
    time(&t);
    info = localtime(&t);
    sprintf(mytime,"%02d月 %02d %02d:%02d ", \
            info->tm_mon+1,info->tm_mday,info->tm_hour,\
            info->tm_min);
    mytime[strlen(mytime)-1] = 0;
    sprintf(sql, "insert into history values ('%s', '%s')", mytime, buf);
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
	}
    sqlite3_free_table(result);
}
//添加用户
void ser_add(int sfd, void * arg, void *arg1)
{
    struct sockaddr_in cin = *((struct sockaddr_in*)arg1);
    struct my_user myuser = *((struct my_user*)arg);
    struct my_staff recv_mystaff; 
    char buf[188] = "";
    char mytime[64] = "";
    char arr[20] = "";
    char arr1[20] = "";
    char brr[20] = "";
    char sql[256] = "";
	char *errmsg = NULL;
    if(!strcmp(myuser.type, "0"))
    {
        strcpy(arr, "管理员");
    }
    else if(!strcmp(myuser.type, "1"))
    {
        strcpy(arr, "普通用户");
    }
    if(recv(sfd, &recv_mystaff, sizeof(recv_mystaff), 0) < 0)
    {
        ERR_MSG("recv");
    }
	sprintf(sql,"insert into staff values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",\
    recv_mystaff.id, recv_mystaff.type, recv_mystaff.name, recv_mystaff.passwd,recv_mystaff.age,\
    recv_mystaff.phone, recv_mystaff.address, recv_mystaff.post, recv_mystaff.time, \
    recv_mystaff.score, recv_mystaff.wage);
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
	}
    strcpy(arr1, "success");
    if(send(sfd, arr1, sizeof(arr1), 0) < 0)
    {
        ERR_MSG("send");
    }

    sprintf(buf, "---%s---%s%s添加了%s用户",myuser.username, arr,myuser.username,recv_mystaff.id);
    struct tm* info = NULL;
    time_t t;
    time(&t);
    info = localtime(&t);
    sprintf(mytime,"%02d月 %02d %02d:%02d ", \
            info->tm_mon+1,info->tm_mday,info->tm_hour,\
            info->tm_min);
    mytime[strlen(mytime)-1] = 0;
    sprintf(sql, "insert into history values ('%s', '%s')", mytime, buf);
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
	}
}
//删除用户
void ser_del(int sfd, void * arg, void *arg1)
{
    struct sockaddr_in cin = *((struct sockaddr_in*)arg1);
    struct my_user myuser = *((struct my_user*)arg);
    struct my_staff recv_mystaff;
    char buf[188] = "";
    char mytime[64] = "";
    char arr[20] = "";
    char arr1[20] = "";
    char brr[20] = "";
    char sql[128] = "";
	char *errmsg = NULL;
    if(!strcmp(myuser.type, "0"))
    {
        strcpy(arr, "管理员");
    }
    else if(!strcmp(myuser.type, "1"))
    {
        strcpy(arr, "普通用户");
    }
    if(recv(sfd, &recv_mystaff, sizeof(recv_mystaff), 0) < 0)
    {
        ERR_MSG("recv");
    }
	sprintf(sql,"delete from staff where id='%s' and name='%s'", recv_mystaff.id, recv_mystaff.name);
	printf("%s\n",sql);
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
	}
    strcpy(arr1, "success");
    if(send(sfd, arr1, sizeof(arr1), 0) < 0)
    {
        ERR_MSG("send");
    }

    sprintf(buf, "---%s---%s%s删除了%s用户",myuser.username, arr,myuser.username,recv_mystaff.id);
    struct tm* info = NULL;
    time_t t;
    time(&t);
    info = localtime(&t);
    sprintf(mytime,"%02d月 %02d %02d:%02d ", \
            info->tm_mon+1,info->tm_mday,info->tm_hour,\
            info->tm_min);
    mytime[strlen(mytime)-1] = 0;
    sprintf(sql, "insert into history values ('%s', '%s')", mytime, buf);
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
	}
}
//历史记录查询
void ser_select_history(int sfd, void * arg, void *arg1)
{
    struct sockaddr_in cin = *((struct sockaddr_in*)arg1);
    struct my_user myuser = *((struct my_user*)arg);
    char buf[128] = "";
    char sql[128] = "";
	char *errmsg = NULL;
	char **result = NULL;
	int row,column;
	sprintf(sql,"select * from history");
	printf("%s\n",sql);
    if(sqlite3_get_table(db, sql, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return;
	}
    int i = 2;
    while(i < ((row+1)*column))
    {
        strcpy(buf, result[i]);
        strcat(buf,result[++i]);
        i++;
        if(send(sfd, buf, sizeof(buf), 0) < 0)
        {
            ERR_MSG("send");
        }
    }
    strcpy(buf, "end");
    if(send(sfd, buf, sizeof(buf), 0) < 0)
    {
        ERR_MSG("send");
    }
    sqlite3_free_table(result);
}

int main(int argc, char const *argv[])
{
    //创建并打开数据库
    if(sqlite3_open("./my.db", &db) != SQLITE_OK)
    {  
        fprintf(stderr, "__%d__ code:%d sqlite3_open:%s\n",__LINE__,
        sqlite3_errcode(db), sqlite3_errmsg(db));
        return -1;
    }
    printf("数据库打开成功\n");
    //创建员工信息表格
    char *errmsg = NULL;
    char sql[256] = "create table if not exists staff (id char primary key, type char, name char, passwd char, age char, phone char, address char, post char, time char, score char, wage char)";
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return -1;
    }
    //创建历史信息表格
    strcpy(sql,"create table if not exists history (time char, message char);");
    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return -1;
    }
    //创建套接字
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0)
    {
        ERR_MSG("socket");
        return -1;
    }
    //允许端口快速复用
    int reuse = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        ERR_MSG("setsockopt");
        return -1;
    }
    //填充地址信息结构体
    struct sockaddr_in sin;
    sin.sin_family         = AF_INET;
    sin.sin_port           = htons(atoi(argv[2]));
    sin.sin_addr.s_addr    = inet_addr(argv[1]);

    //将IP和端口绑定到套接字上
    if(bind(sfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        ERR_MSG("bind");
        return -1;
    }
    //将套接字设置为被动监听状态
    if(listen(sfd, 10) < 0)
    {
        ERR_MSG("listen");
        return -1;
    }

    struct sockaddr_in cin;
    socklen_t addrlen = sizeof(cin);

    int newfd = 0;
    pthread_t tid;
    struct msg cliInfo;

    while(1)
    {
        //主线程负责连接
        //取出新的文件描述符
        newfd = accept(sfd, (struct sockaddr *)&cin, &addrlen);
        if(newfd < 0)
        {
            ERR_MSG("accept");
            return -1;
        }
        printf("[%s:%d] newfd=%d 连接成功\n",inet_ntoa(cin.sin_addr), ntohs(cin.sin_port),newfd);

        cliInfo.newfd = newfd;
        cliInfo.cin = cin;
        //创建分支线程
        if(pthread_create(&tid, NULL, pthread_ser, &cliInfo) != 0)
        {
            ERR_MSG("pthread_create");
            return -1;
        }
    }
    close(sfd);
    if(sqlite3_close(db) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ code:%d sqlite3_open:%s\n", __LINE__, \
				sqlite3_errcode(db), sqlite3_errmsg(db));
		return -1;
	}
	printf("关闭数据库成功\n");
    return 0;
}
