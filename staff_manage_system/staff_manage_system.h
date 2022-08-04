#ifndef __STAFF_MANAGE_SYSTEM_H__
#define __STAFF_MANAGE_SYSTEM_H__

#define ERR_MSG(msg) do{\
    fprintf(stderr,"__%d__:",__LINE__);\
    perror(msg);\
}while(0)

struct my_user
{
    int num; 
    char username[10];
    char userpswd[10];
    char arr[12];
    char type[2];
    char id[5];
};

struct my_staff
{
    int eflag;
    char type[2];
    char id[5];
    char name[10];
    char age[3];
    char address[10];
    char phone[11];
    char post[10];
    char wage[6];
    char time[11];
    char score[2];
    char passwd[7];
};
/****************************cli.c********************************/
//选择界面
void start_cli(int sfd, void * arg);
//普通用户选择
void normal_cli_select(void *user_cli, int sfd, void * arg);
//普通用户登录
void normal_cli_entry(int sfd, void *arg);
//管理员登录
void manager_cli_entry(int sfd, void * arg);
//管理员选项
void manager_cli_select(void *user_cli, int sfd, void * arg);
/*****************************************************************/

/****************************ser.c********************************/
//用户登录
void ser_entry(int sfd, void * arg, void *arg1);
//用户查询
void ser_select(int sfd, void * arg, void *arg1);
//修改信息
void ser_modify(int sfd, void * arg, void *arg1);
//添加用户
void ser_add(int sfd, void * arg, void *arg1);
//删除用户
void ser_del(int sfd, void * arg, void *arg1);
//历史记录查询
void ser_select_history(int sfd, void * arg, void *arg1);
/*****************************************************************/



#endif