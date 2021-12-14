### WordPress更改域名

前一阵子我的域名到期了，昨天我刚刚换了一个新的域名

很痛苦的是，wordpress的数据库中绑定的还是我以前的域名。这就导致我即便是换了域名，我还是登录不了我的博客

经过了再三的折腾，我终于成功的把域名迁移了过来。下面就具体说一说我是怎么做的

我的博客是部署在远程服务器上，用docker部署的wordpress和mysql

进到服务器中，首先登录到mysql容器内部

执行这个

```shell
docker exec -it blog_db_1 bash
```

其中`blog_db_1`是你的mysql容器的名字，用过`docker ps`查看正在运行的docker容器

登录进来以后，再进入到数据库中

```shell
mysql -u name -p
```

`name`就是你的mysql的用户名，然后再输入密码就进来了

```shell
show databases;
```

查看数据库

![20211214132112](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20211214132112.png)

```shell
use wordpress;
```

进入到wordpress中

然后用

```shell
show tables;
```

可以看到我们所有的表

这里我们主要要更改的有三个表

在此之前，我们可以先查看之前的域名

```sql
select * from wp_options where option_name = 'home' or option_name = 'siteurl';
```

![20211214132410](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20211214132410.png)

这里我已经更改了我自己的域名，如果你没有更改的话，`option_value`这里就是原本的域名

确认了我们现在的域名以后，我们可以进行一系列的修改

开启事务，这样可以让我们在误操作的时候进行回滚

```sql
begin;
UPDATE wp_options SET option_value = replace( option_value, 'http://neub607.xyz', 'http://heavensheep.xyz' ) WHERE option_name = 'home' OR option_name = 'siteurl';
UPDATE wp_posts SET post_content = replace( post_content, 'http://neub607.xyz', 'http://heavensheep.xyz' ) ;
UPDATE wp_posts SET guid = replace( guid, 'http://neub607.xyz', 'http://heavensheep.xyz' ) ;
commit;
```

这是我执行的语句，你可以把`http://neub607.xyz`换成你自己的旧域名

然后把`http://heavensheep.xyz`换成新的域名

注意一句一句的执行，查看一下每一句的执行结果，最后确认无误后再commit

如果你的操作正确的话，你的博客在修改之后就可以直接登录进去了

下次换域名的时候要记得提前申请域名