#!bin/bash

'''
题目描述：
    字符串包含[] {} ()　判断是否满足规则类似数学运算成对出现如：
    input: "()"
    output: true

    input: "{()}"
    output: true

    input: "{[(})]"
    output: false 

思路：
    类似设计简单计算器的方法，用栈实现，先顺序入栈，如果下一个匹配栈中栈顶元素，则出栈

'''

class solution:
    def test1(self,s):
        stack = []
        map = {
            "(":")",
            "[":"]",
            "{":"}"
        }

        for x in s:
            if x in map:
                stack.append(map[x])
            else
                if len(stack) != 0:
                    top = stack.pop()
                    if x != top:
                        return False
                    else:
                        continue
                else:
                    return False
            return len(stack) == 0


