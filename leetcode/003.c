/*
给定一个字符串，请你找出其中不含有重复字符的 最长子串 的长度。

示例 1:

输入: "abcabcbb"
输出: 3 
解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。

示例 2:

输入: "bbbbb"
输出: 1
解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。

示例 3:

输入: "pwwkew"
输出: 3
解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
     请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。



*/

#include<stdlib.h>
#include<stdio.h>



int lengthOfLongestSubstring(char* s) {
	char *p1 , *p2, *p3;
	char len = 0;
	char len1 = 0;
	p1=p2=p3=s;
    if(strlen(s)==1)
        return 1;
	while(*p2!='\0')
	{
		p3 = p1;
		len1 = 1;
		while(p3!=p2)
		{
			if(*p3==*p2)
			{
				if(len<len1)
					len = len1;
				p1 = ++p3;
				break;
			}
			len1++;
			p3++;
			if(len<len1)
				len = len1;
		}
		p2++;

	}
	return len;
    
}


void  main()
{
	char str[20] = "abcabcbbbbbabcdef";
	int len = lengthOfLongestSubstring(str);
	printf("len = %d\n",len);
	
}
