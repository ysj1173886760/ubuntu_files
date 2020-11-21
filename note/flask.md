w3cschool讲的很清楚，这里记录一下我认为比较关键的点

使用`ctrl+z`是挂起这个程序，更准确的说是从终端中退出这个程序，但进程仍然在继续

使用`ctrl+c`在终端中终止这个程序

在app.run()中设置参数debug=True后，每次更改代码都会使服务器重新加载

在规则参数中添加变量，可以动态构建url，这个变量将作为参数传递给与该url相关联的函数

flask路由，即route装饰器用于将url绑定到函数

`/flask可以使用/flask访问，但不能用/flask/访问`

`/flask/既可以用/flask/也可以用/flask访问`

url_for 第一个参数为函数名，剩下的参数为关键字参数，用于对应URL的变量，他会返回对应的URL

flask中的http方法，首先需要了解html表单的概念，这个可以取w3cschool里查一下，很好理解

其中，request的form是从POST和PUT请求解析的MultiDict，args也是一个MultiDict，使用args.get来获得对应的键值

POST一般用于传输加密的信息，而GET的参数是会在URL的？之后出现的参数

![image-20201118180438676](/home/sheep/.config/Typora/typora-user-images/image-20201118180438676.png)

request对象的重要属性

url_for的其他使用方式

访问静态文件时，第一个参数使用static，第二个为filename='xxx.css'

还可以指定参数endpoint，flask中通过endpoint来找到视图函数，所以直接输入endpoint可以直接找到对应的视图函数

有关文件上传，这里讲的很细就不再赘述了[文件上传](https://www.w3cschool.cn/flask/flask_file_uploading.html)

flask的请求与响应

请求对象就是request对象，是客户端向服务器请求信息用的，服务器通过请求对象获得客户端的请求

响应对象是response对象，是服务器响应给客户端的内容，最开始的return 'hello world' 就是响应给客户端字符串，也可以响应模板（render_template）或者响应重定向（redirect），同样的我们可以响应response对象，一般是用于设置cookie