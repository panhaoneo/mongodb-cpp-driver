# mongodb-cpp-driver
Package cpp mongodb driver reference to Tencent Tars TC_Mysql
参考腾讯开源的tars库封装mysql的方式。tc_mysql 有一个缺点是没有连接池，[mongodb_cxx](http://mongocxx.org/)自带连接池。
C++在数据库驱动方面可以考虑学习下java的jdbc接口标准，数据库都用这个标准驱动，减少学习成本，不用每个库都花时间学习。学一个标准接口全部适用。

# 组件
- 封装mongodb driver 的连接池
- 组装数据方式使用map 键值对方式
- 封装常用操作接口：insert，update，find

# 待做项
- 学习jdbc接口标准，看是否合适C++实现一套。在现有的driver基础上做一个标准。
- 扩展mongo的find接口，准备使用json（jsoncpp）来构造doc进行多元化查找。
- 性能优化：链接池可以复用。

# 总结博文系列
1. bson 数据
2. stream和basic两个大类
3. 连接池
4. 增删改操作
5. 查操作
6. 索引index
7. 性能
