#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "staff_manage_system.h"

//管理员登录
void manager_cli_entry(int sfd, void * arg)
{
    struct sockaddr_in sin = *((struct sockaddr_in *)arg);
    struct my_user myuser; 
    char buf[128] = "";
    myuser.num = 0;
    strcpy(myuser.type, "0");
    while(1)
    {
        printf("请输入用户名:");
        scanf("%s",myuser.username);
        printf("请输入密码:");
        scanf("%s",myuser.userpswd);
        if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
        {
            ERR_MSG("send");
            return;
        }
        if(recv(sfd, buf, sizeof(buf), 0) < 0)
        {
            ERR_MSG("recv");
            return;
        }
        if(!(strcmp(buf, "success")))
        {
            printf("亲爱的管理员，欢迎您登陆员工管理系统!\n");
            manager_cli_select(&myuser, sfd, &sin);
            break;
        }
        else
        {
            printf("登陆失败,请重新登录\n");
        }
    }
}
//管理员选项
void manager_cli_select(void *user_cli, int sfd, void * arg)
{
    struct sockaddr_in sin = *((struct sockaddr_in *)arg);
    struct my_user myuser = *((struct my_user *)user_cli);
    struct my_staff rev_mystaff;
    struct my_staff mystaff;
    char buf[128] = "";
    char brr[10] = "";
    char arr1[20] = "";
    int a = 0,b = 0,c = 0;
    char flag;
    char flag1;
    while(1)
    {
    V1:
        printf("**************************************************************\n");
        printf("****1.查询 2.修改 3.添加用户 4.删除用户 5.查询历史记录 6.退出****\n");
        printf("**************************************************************\n");
        printf("请输入你的选择:");
        scanf("%d",&a);
        getchar();
        if(a == 6)
        {
            goto V2;
            break;
        }
        switch(a)
        {
        case 1:
            printf("*****************************************************\n");
            printf("***********1.按人名查询  2.查询所有  3.退出***********\n");
            printf("*****************************************************\n");
            printf("请输入你的选择:");
            scanf("%d",&b);
            getchar();
            if(b == 3)
            {
                goto V1;
                break;
            }
            switch(b)
            {
                case 1:
                    myuser.num = 1;
                    if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                    {
                        ERR_MSG("send");
                        return;
                    }
                    printf("请输入要查询的名字:");
                    scanf("%s",myuser.arr);
                    myuser.num = 101;
                    if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                    {
                        ERR_MSG("send");
                        return;
                    }
                    if(recv(sfd, &rev_mystaff, sizeof(rev_mystaff), 0) < 0)
                    {
                        ERR_MSG("recv");
                    }
                    printf("工号\t用户类型\t姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t工资\n");
                    printf("=====================================================================\n");
                    printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",rev_mystaff.id, rev_mystaff.type,
                    rev_mystaff.name, rev_mystaff.passwd, rev_mystaff.age, rev_mystaff.phone,
                    rev_mystaff.address, rev_mystaff.post, rev_mystaff.time, rev_mystaff.score, 
                    rev_mystaff.wage);
                    break;
                case 2:

                    myuser.num = 1;
                    if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                    {
                        ERR_MSG("send");
                        return;
                    }
                    usleep(1000);
                    myuser.num = 102;
                    if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                    {
                        ERR_MSG("send");
                        return;
                    }
                    while(1)
                    {
                        if(recv(sfd, &rev_mystaff, sizeof(rev_mystaff), 0) < 0)
                        {
                            ERR_MSG("recv");
                        }

                        if(rev_mystaff.eflag != 0)
                        {
                            printf("工号\t用户类型\t姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t工资\n");
                            printf("=====================================================================\n");
                            printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",rev_mystaff.id, rev_mystaff.type,
                            rev_mystaff.name, rev_mystaff.passwd, rev_mystaff.age, rev_mystaff.phone,
                            rev_mystaff.address, rev_mystaff.post, rev_mystaff.time, rev_mystaff.score, 
                            rev_mystaff.wage);
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                default:
                    printf("请重新输入\n");
                    break;
            }
            break;
        case 2:
            while(1)
            {
                printf("请输入您要修改的工号:");
                scanf("%s",myuser.id);
                getchar();
                myuser.num = 2;
                if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                {
                    ERR_MSG("send");
                    return;
                }
                printf("**********************************************\n");
                printf("********1.姓名 2.年龄 3.家庭住址 4.电话********\n");
                printf("********5.职位 6.工资 7.入职年月 8.评级********\n"); 
                printf("****************9.密码 10.退出*****************\n");
                printf("**********************************************\n");
                printf("请输入你的选择:");
                scanf("%d",&c);
                getchar();
                if(c == 10)
                {
                    goto V1;
                    break;
                }

                if(c == 1)
                {
                    myuser.num = 21;
                    printf("请输入要修改的姓名:");
                }
                else if(c == 2)
                {
                    myuser.num = 22;
                    printf("请输入要修改的年龄:"); 
                }
                else if(c == 3)
                {
                    myuser.num = 23;
                    printf("请输入要修改的家庭住址:"); 
                }
                else if(c == 4)
                {
                    myuser.num = 24;
                    printf("请输入要修改的电话:"); 
                }
                else if(c == 5)
                {
                    myuser.num = 25;
                    printf("请输入要修改的职位:");
                }
                else if(c == 6)
                {
                    myuser.num = 26;
                    printf("请输入要修改的工资:"); 
                }
                else if(c == 7)
                {
                    myuser.num = 27;
                    printf("请输入要修改的入职年月:"); 
                }
                else if(c == 8)
                {
                    myuser.num = 28;
                    printf("请输入要修改的评级:"); 
                }
                else if(c == 9)
                {
                    myuser.num = 29;
                    printf("请输入要修改的密码:");
                }
                else 
                {
                    printf("请重新输入\n");
                    continue;
                }  
                scanf("%s",myuser.arr); 
                getchar();                  
                printf("%s\n",myuser.arr);       
                if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                {
                    ERR_MSG("send");
                    return;
                }
                usleep(1000);
                bzero(arr1, sizeof(arr1));
                if(recv(sfd, arr1, sizeof(arr1), 0) < 0)
                {
                    ERR_MSG("recv");
                }

                if(!strcmp(arr1, "success"))
                {
                    printf("数据库修改成功!修改结束\n");
                    break;
                }
                
                else if(strcmp(arr1, "success"))
                {
                    printf("%s\n",arr1);
                    printf("数据库修改失败!修改结束\n");
                    break;
                }
            }
            break;
        case 3:
            while(1){
                myuser.num = 3;
                if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                {
                    ERR_MSG("send");
                    return;
                } 
                printf("请输入工号:");
                scanf("%s",mystaff.id);
                getchar();
                printf("您输入的工号是:%s\n",mystaff.id);
                printf("工号信息一旦录入无法更改，请确认您所输入的是否正确!(y/n):");
                scanf("%c",&flag);
                getchar();
                if(flag == 'y' || flag == 'Y')
                {
                    printf("请输入用户名:");
                    scanf("%s",mystaff.name);
                    getchar();
                    printf("请输入用户密码:");
                    scanf("%s",mystaff.passwd);
                    getchar();
                    printf("请输入年龄:");
                    scanf("%s",mystaff.age);  
                    getchar();
                    printf("请输入电话:");
                    scanf("%s",mystaff.phone);
                    getchar();
                    printf("请输入家庭住址:");
                    scanf("%s",mystaff.address);
                    getchar();
                    printf("请输入职位:");
                    scanf("%s",mystaff.post);
                    getchar();
                    printf("请输入入职日期:");
                    scanf("%s",mystaff.time);
                    getchar();
                    printf("请输入评级:");
                    scanf("%s",mystaff.score);
                    getchar();
                    printf("请输入工资:");
                    scanf("%s",mystaff.wage);    
                    getchar();
                    printf("是否为管理员:(y/n):");  
                    scanf("%c",&flag);
                    getchar();                    
                    if(flag == 'y' || flag == 'Y')
                    {
                        strcpy(mystaff.type, "0");
                    }       
                    else 
                    {
                        strcpy(mystaff.type, "1");
                    }   
                    if(send(sfd, &mystaff, sizeof(mystaff), 0) < 0)
                    {
                        ERR_MSG("send");
                        return;
                    }
                    usleep(1000);
                    if(recv(sfd, arr1, sizeof(arr1), 0) < 0)
                    {
                        ERR_MSG("recv");
                    }
                    if(!strcmp(arr1, "success"))
                    {
                        printf("数据库修改成功!是否继续添加员工:(Y/N):");

                        scanf("%c",&flag1);
                        getchar();
                        if(flag == 'y' || flag1 == 'Y')
                        {
                            continue;
                        }      
                        else if(flag1 == 'n' || flag1 == 'N')
                        {
                            goto V1;
                            break;
                        } 
                    }  
                    else
                    {
                        printf("falied\n");
                    }                      
                }
                else if(flag == 'n' || flag == 'N')
                {
                    goto V1;
                    break;
                }
            }
            break;
        case 4:
            myuser.num = 4;
            if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
            {
                ERR_MSG("send");
                return;
            }
            printf("请输入要删除的用户工号:");
            scanf("%s",mystaff.id);
            getchar();
            printf("请输入要删除的用户名:");
            scanf("%s",mystaff.name);
            getchar();
            if(send(sfd, &mystaff, sizeof(mystaff), 0) < 0)
            {
                ERR_MSG("send");
                return;
            }
            if(recv(sfd, arr1, sizeof(arr1), 0) < 0)
            {
                ERR_MSG("recv");
            }
            if(!strcmp(arr1, "success"))  
            {
                printf("数据库修改成功!删除工号为:%s的用户\n",mystaff.id);
            }              
        
            break;
        case 5:
            myuser.num = 5;
            if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
            {
                ERR_MSG("send");
                return;
            }
            while(1)
            {
                if(recv(sfd, buf, sizeof(buf), 0) < 0)
                {
                    ERR_MSG("recv");
                }
                if(strcmp(buf, "end"))
                {
                    printf("%s\n", buf);
                }
                else
                {
                    break;
                }
            }
            break;
        default:
            printf("请重新输入\n");
            break;
        }
    }
V2:
    return;
}

//普通用户登录
void normal_cli_entry(int sfd, void *arg)
{
    struct sockaddr_in sin = *((struct sockaddr_in *)arg);
    struct my_user myuser; 
    char buf[128] = "";
    myuser.num = 10;
    strcpy(myuser.type, "1");
    while(1)
    {
        printf("请输入用户名:");
        scanf("%s",myuser.username);
        getchar();
        printf("请输入密码:");
        scanf("%s",myuser.userpswd);
        getchar();
        if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
        {
            ERR_MSG("send");
            return;
        }
        if(recv(sfd, buf, sizeof(buf), 0) < 0)
        {
            ERR_MSG("recv");
            return;
        }
        if(!(strcmp(buf, "success")))
        {
            normal_cli_select(&myuser, sfd, &sin);
            break;
        }
    }
}

//普通用户选择
void normal_cli_select(void *user_cli, int sfd, void * arg)
{
    struct sockaddr_in sin = *((struct sockaddr_in *)arg);
    struct my_user myuser = *((struct my_user *)user_cli);
    struct my_staff rev_mystaff;
    char buf[128] = "";
    char arr1[20] = "";
    int a = 0,b = 0;
    while(1)
    {
    N1:
        printf("*******************************************\n");
        printf("***********1.查询  2.修改  3.退出***********\n");
        printf("*******************************************\n");
        printf("请输入你的选择:");
        scanf("%d",&a);
        getchar();
        if(a == 3)
        {
            goto N2;
            break;
        }
        switch(a)
        {
        case 1:
            myuser.num = 111;
            if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
            {
                ERR_MSG("send");
                return;
            }
            if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
            {
                ERR_MSG("send");
                return;
            }
            if(recv(sfd, &rev_mystaff, sizeof(rev_mystaff), 0) < 0)
            {
                ERR_MSG("recv");
            }
            printf("工号\t用户类型\t姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t工资\n");
            printf("=====================================================================\n");
            printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",rev_mystaff.id, rev_mystaff.type,
            rev_mystaff.name, rev_mystaff.passwd, rev_mystaff.age, rev_mystaff.phone,
            rev_mystaff.address, rev_mystaff.post, rev_mystaff.time, rev_mystaff.score, 
            rev_mystaff.wage);
            break;
        case 2:
            myuser.num = 12;
            if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
            {
                ERR_MSG("send");
                return;
            }
            while(1)
            {
                printf("**********************************************\n");
                printf("********1.姓名 2.年龄 3.家庭住址 4.电话********\n");
                printf("********5.职位 6.工资 7.入职年月 8.评级********\n"); 
                printf("****************9.密码 10.退出****************\n");
                printf("**********************************************\n");
                printf("请输入你的选择:");
                scanf("%d",&b);
                if(b == 10)
                {
                    goto N1;
                    break;
                }
                switch(b)
                {
                    case 1:
                        myuser.num = 121;
                        printf("请输入要修改的姓名:");
                        break;
                    case 2:
                        myuser.num = 122;
                        printf("请输入要修改的年龄:");
                        break;
                    case 3:
                        myuser.num = 123;
                        printf("请输入要修改的家庭住址:");
                        break;
                    case 4:
                        myuser.num = 124;
                        printf("请输入要修改的电话:");
                        break;
                    case 5:
                        myuser.num = 125;
                        printf("请输入要修改的职位:");
                        break;
                    case 6:
                        myuser.num = 126;
                        printf("请输入要修改的工资:");
                        break;
                    case 7:
                        myuser.num = 127;
                        printf("请输入要修改的入职年月:");
                        break;
                    case 8:
                        myuser.num = 128;
                        printf("请输入要修改的评级:");
                        break;
                    case 9:
                        myuser.num = 129;
                        printf("请输入要修改的密码:");
                        break;
                    default:
                        printf("请重新输入\n");
                        continue;
                }
                scanf("%s",myuser.arr);      
                getchar();       
                if(send(sfd, &myuser, sizeof(myuser), 0) < 0)
                {
                    ERR_MSG("send");
                    return;
                }
                if(recv(sfd, arr1, sizeof(arr1), 0) < 0)
                {
                    ERR_MSG("recv");
                }
                if(!strcmp(arr1, "success"))
                {
                    printf("数据库修改成功!修改结束\n");
                    break;
                }
            }
            break;
        default:
            printf("请重新输入\n");
            break;
        }
    }
N2:
    return ;
}

//选择界面
void start_cli(int sfd, void * arg)
{
    struct sockaddr_in sin = *((struct sockaddr_in *)arg);
    int a = 0;
    while(1)
    {
        printf("********************************************\n");
        printf("*****1.管理员模式  2.普通用户模式  3.退出*****\n");
        printf("********************************************\n");
        printf("请输入>");
        scanf("%d", &a);
        getchar();
        if(a == 3)
        {
            break;
        }
        switch(a)
        {
        case 1:
            manager_cli_entry(sfd, &sin);
            break;
        case 2:
            normal_cli_entry(sfd, &sin);
            break;
        default:
            printf("请重新输入\n");
            break;
        }
    }

}

int main(int argc, char const *argv[])
{
    //创建流式套接字
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0)
    {
        ERR_MSG("socket");
        return -1;
    }
    //填充服务器的IP和端口号
    struct sockaddr_in sin;
    sin.sin_family       = AF_INET;
    sin.sin_port         = htons(atoi(argv[2]));
    sin.sin_addr.s_addr  = inet_addr(argv[1]);

    //连接服务器
    if(connect(sfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        ERR_MSG("connect");
        return -1;
    }
    start_cli(sfd, &sin);

    return 0;
}