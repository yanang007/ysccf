# ysccf
Yanang's simple compiler of compiler framwork with a simple GUI inplemented using Qt 5.  
颜昂的文法编译器框架，用Qt5实现了简易的图形界面。

A simple LR(0) grammar's parser has been implemented and testified.  
已实现验证LR(0)文法的解析。

LALR(1) parser and code generation are in progress.  
计划实现LALR(1)文法的解析与代码生成。

Todo：

文法编译器：

* [ ] 实现属性（AttributedStatement） 
```
[ignore]
%token space "\s+"
...
<definition> ::= "class" [userDefinedType]identifier <definitionBody> 
// 当依据这条产生式规约时可以读取到对应字段加入自定义类型表
// 然后在接下来的词法解析中得以应用
...
```

* [ ] 实现一些语法糖
<table>
    <tr>
        <td>before</td>
        <td>after</td>
    </tr>
    <tr>
        <td><pre>
&ltrule> ::= &ltproducer> deducer &ltproduced> &ltorProduced> delimiter;
&ltorProduced> ::= or &ltproduced> &ltorProduced> | null;
        </pre></td>
        <td><pre>
&ltrule> ::= &ltproducer> deducer &ltproduced>+ delimiter;
        </pre></td>
    </tr> 
</table>



