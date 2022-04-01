# -*- coding: utf-8 -*-
"""
Created on Wed Jul  8 09:06:20 2020

@author: dell
"""
import os, shutil

def copyfile(src_file, dst_path):  #将文件拷贝至dst_path目录下
    if not os.path.isfile(src_file):
        print("%s not exist!"%(src_file))
    else:
        if not os.path.exists(dst_path):
            os.makedirs(dst_path)                #创建路径
        shutil.copy(src_file,dst_path)      #复制文件
        print("copy %s -> %s"%( src_file,dst_path))


last_path=os.path.abspath(os.path.dirname(os.getcwd()))  #上级目录


libirparse_dll=last_path+'\libirparse\Release\\x64\dll\libirparse.dll'
libirprocess_dll=last_path+'\libirprocess\Release\\x64\dll\libirprocess.dll'
libirtemp_dll=last_path+'\libirtemp\Release\\x64\dll\libirtemp.dll'
libiruvc_dll=last_path+'\libiruvc\Release\\x64\dll\libiruvc.dll'
src_dll_list=[libirparse_dll,libirprocess_dll,libirtemp_dll,libiruvc_dll]
dst_dll_path='libir_sample\\x64\Debug'

libirparse_lib=last_path+'\libirparse\Release\\x64\dll\libirparse.lib'
libirprocess_lib=last_path+'\libirprocess\Release\\x64\dll\libirprocess.lib'
libirtemp_lib=last_path+'\libirtemp\Release\\x64\dll\libirtemp.lib'
libiruvc_lib=last_path+'\libiruvc\Release\\x64\dll\libiruvc.lib'
src_lib_list=[libirparse_lib,libirprocess_lib,libirtemp_lib,libiruvc_lib]
dst_lib_path=os.getcwd()+'\libs'

libirparse_header=last_path+'\\libirparse\\include\\libirparse.h'
libirprocess_header=last_path+'\\libirprocess\\include\\libirprocess.h'
libirtemp_header=last_path+'\\libirtemp\\include\\libirtemp.h'
libiruvc_header=last_path+'\libiruvc\\include\\libiruvc.h'
tiny1bcmd_header=last_path+'\libiruvc\\include\\tiny1bcmd.h'
falconcmd_header=last_path+'\libiruvc\\include\\falcon_cmd.h'
allconfig_header=last_path+'\libiruvc\\include\\all_config.h'

src_header_list=[libirparse_header, libirprocess_header, libirtemp_header,\
                 libiruvc_header, tiny1bcmd_header, falconcmd_header, allconfig_header]
dst_header_path=os.getcwd()+'\include'


for i in range(len(src_dll_list)):
    copyfile(src_dll_list[i],dst_dll_path)
    
for i in range(len(src_dll_list)):
    copyfile(src_lib_list[i],dst_lib_path)
    
for i in range(len(src_header_list)):
    copyfile(src_header_list[i],dst_header_path)

input("copy completed!! Press any key to exit...")
    
