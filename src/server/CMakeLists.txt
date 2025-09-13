# 定义变量SRC_LIST:存储当前目录下的所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)

# 由所有的源码生成ChatServer可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})
# 指定ChatServer可执行文件链接时所依赖的库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread)