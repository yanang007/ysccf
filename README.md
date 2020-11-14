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
<classDefinition> ::= 
        "class"
        identifier -> [userDefinedType("definedClassName")] 
        <definitionBody>
        ";"
        ;
// 当依据这条产生式规约时可以读取到对应identifier的token加入自定义类型表
// token类型转化为definedClass而不再是identifier
// 然后在接下来的词法解析中得以应用
<variableDefinition> ::= 
        <definedClassName>
        identifier
        ";"
        ;

<producer> ::= beforeVn [userDefinedType]identifier afterVn;

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


* [ ] 加强的递归展开（可以引入[inline]属性），指示当前符号会直接在其所有引用位置展开

<table>
    <tr>
        <td>before</td>
        <td>after</td>
    </tr>
    <tr>
        <td><pre>
─── statement
    └── rule
        ├── producer
        │   └── VnExpr
        │       ├── beforeVn
        │       ├── userDefinedType(SomeVnName)
        │       └── afterVn
        ├── deducer
        ├── produced      // 现在可以通过链式展开，
        │   ├── ...       // 将非终结符下重复出现的其它递归项展开
        │   └── ...       
        ├── orProduced    
        │   ├── produced  // 但是从语义上来说，
        │   │   ├── ...   // orProduced的子节点应该直接展开到上一层
        │   │   └── ...   
        │   └── produced  // 不过递归的非终结符需要指引才能在上一层就完成展开
        │       ├── ...   
        │       └── ...
        └── delimiter
        </pre></td>
        <td><pre>
─── statement
    └── rule
        ├── producer
        │   └── VnExpr
        │       ├── beforeVn
        │       ├── userDefinedType(SomeVnName)
        │       └── afterVn
        ├── deducer
        ├── produced  // 即 &ltproduced>* 
        │   ├── ...
        │   └── ...
        ├── produced
        │   ├── ...
        │   └── ...
        ├── produced
        │   ├── ...
        │   └── ...
        └── delimiter
        </pre></td>
    </tr> 
</table>

* [ ] compilerFrontendParallel，使用队列实现词法和文法解析的并行执行

<table>
    <tr>
        <td>before</td>
        <td><pre>
lexer -> custom steps -> parser -> custom steps -> user
        </pre></td>
    </tr>
    <tr>
        <td>after</td>
        <td><pre>
lexer -> custom steps -> token queue
fetched from token queue -> custom steps -> parser -> custom steps -> user
        </pre></td>
    </tr> 
</table>

* [ ] 算符分析方法