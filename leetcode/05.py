#!/bin/bash

'''
题目描述：
    给定一个大小有序但元素重复的数组，返回元素不重复的数组和长度

    如：
    input:{0,1,1,2,3,3,3}
    output:{0,1,2,3} len = 4

思路s
    最容易想到的方法是双指针，参考插入排序操作指针的方式
'''


class solution:
    def test1(self,list)
        if list:
            p1 = 0
            for p2 in range(1,len(list)):
                if list[p2] == list[p1]:
                    continue
                else:
                    p1++
                    list[p1] = list[p2]
            return p1+1
        eles:s
            return 0
