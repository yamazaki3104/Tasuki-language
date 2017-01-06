

#include "TasukiRun.hpp"

#include "TasukiParse.hpp" // TasukiParseSyntax / TasukiInterpreter

#undef NDEBUG
#include <cassert> // assert()

//---------------------------------------------------------
// 単体テストコード

int unit_test()
{
    //----------------
    {
        TasukiParseSyntax syn( "a : 123" ) ;    // 構文解析

        assert( syn.syntax == "[define a 123 ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "bbb :: 'hello'" ) ;    // 構文解析

        assert( syn.syntax == "[public bbb 'hello' ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/.co <- 123" ) ;    // 構文解析

        assert( syn.syntax == "[push [match / co ] 123 ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "123" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/.co <- 123 + 234 + 567 + 678" ) ;    // 構文解析

        assert( syn.syntax == "[push [match / co ] [operator + 123 [operator + 234 [operator + 567 678 ] ] ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/.co <- 123 + 234 * 567 - 678" ) ;    // 構文解析

        assert( syn.syntax == "[push [match / co ] [operator - [operator + 123 [operator * 234 567 ] ] 678 ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "a.call().call()" ) ;    // 構文解析

        assert( syn.syntax == "[call [call a [scope ] ] [scope ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "a.call( 1 ).call( 2 )" ) ;    // 構文解析

        assert( syn.syntax == "[call [call a [scope 1 ] ] [scope 2 ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/.co <- (if -true ?? 'true' !! 'false' )" ) ;    // 構文解析

        assert( syn.syntax == "[push [match / co ] [if [scope -true ] [scope 'true' ] [scope 'false' ] ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "true" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 2 == 5 \n"
            "/.co <- (if 2 == 5 ?? 'true' !! 'false' )\n"
        ) ;    // 構文解析

        assert( syn.syntax == "[push [match / co ] [compare == 2 5 ] ] [push [match / co ] [if [scope [compare == 2 5 ] ] [scope 'true' ] [scope 'false' ] ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "-falsefalse" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "(if 123 456 ?? 789 123 !! 345 567 )" ) ; // 構文解析

        assert( syn.syntax == "[if [scope 123 456 ] [scope 789 123 ] [scope 345 567 ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "( n <= 1 ?? 1 !! n * factorial.call( n - 1 ) )" ) ; // 構文解析

        assert( syn.syntax == "[if [scope [compare <= n 1 ] ] [scope 1 ] [scope [operator * n [call factorial [scope [operator - n 1 ] ] ] ] ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        /* undefine の処理が入っていない todo

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
        */
    }

    //----------------
    {
        TasukiParseSyntax syn( "fact : ( n : arg.0 ( ddd ) )" ) ; // 構文解析

        assert( syn.syntax == "[define fact [scope [define n [match arg 0 ] ] [scope ddd ] ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 0.type   ---> int\n"
            "/.co <- 1.0.type ---> float\n"
        ) ; // 構文解析

        assert( syn.syntax == "[push [match / co ] [match 0 type ] ] [push [match / co ] [match [match 1 0 ] type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "intfloat" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- ( 1 2 3 ).type    --> int \n"
            "/.co <- ( 1 2 3.0 ).type  --> float \n"
            "/.co <- ( 1 2 '3' ).type  --> str \n"
            "/.co <- [ 1 2 3 ].type    --> list \n"
            "/.co <- { 1 2 3 }.type    --> stream \n"
        ) ; // 構文解析

        assert( syn.syntax == "[push [match / co ] [match [scope 1 2 3 ] type ] ] "
            "[push [match / co ] [match [scope 1 2 [match 3 0 ] ] type ] ] "
            "[push [match / co ] [match [scope 1 2 '3' ] type ] ] "
            "[push [match / co ] [match [list 1 2 3 ] type ] ] "
            "[push [match / co ] [match [stream 1 2 3 ] type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "intfloatstrliststream" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/break xxx zzz to yyy -/" ) ; // 構文解析

        assert( syn.syntax == "[break [list xxx zzz ] yyy ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/break if n < 123 -/" ) ; // 構文解析

        assert( syn.syntax == "[if [scope [compare < n 123 ] ] [break [list ] ] [scope ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn( "/break 456 789 if n >= 123 -/" ) ; // 構文解析

        assert( syn.syntax == "[if [scope [compare >= n 123 ] ] [break [list 456 789 ] ] [scope ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }
    
    //----------------
    {
        TasukiParseSyntax syn(
            "i : 123 \n"
            "/.co <- i.type \n"     // --> int
            "/.co <- 456.type \n"   // --> int
        ) ; // 構文解析

        assert( syn.syntax == "[define i 123 ] [push [match / co ] [match i type ] ] [push [match / co ] [match 456 type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "intint" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }
    
    //----------------
    {
        TasukiParseSyntax syn(
            "i : [ 123 ] \n"
            "/.co <- i.type \n"         // --> list
            "/.co <- [ 456 ].type \n"   // --> list
        ) ; // 構文解析

        assert( syn.syntax == "[define i [list 123 ] ] [push [match / co ] [match i type ] ] [push [match / co ] [match [list 456 ] type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "listlist" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn( 
            "--- 階乗 \n"
            "factorial : ( \n"
            "    n : arg.0 \n"
            "    ( n <= 1 ?? 1 !! n * factorial.call( n - 1 ) ) \n"
            ") \n"
            "\n"
            "/.co <- factorial.call( 3 ) --->   6 \n"
            "/.co <- factorial.call( 6 ) ---> 720 \n"
        ) ;

        assert( syn.syntax == "[define factorial [scope [define n [match arg 0 ] ] [if [scope [compare <= n 1 ] ] [scope 1 ] [scope [operator * n [call factorial [scope [operator - n 1 ] ] ] ] ] ] ] ] [push [match / co ] [call factorial [scope 3 ] ] ] [push [match / co ] [call factorial [scope 6 ] ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "6720" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn(
            "--- Fizz Buzz 問題 \n"
            "/.co <- 20.each( \n"
            "    n : it + 1 \n"
            "    (if n % 15 == 0 ?? 'FizzBuzz' !! \n"
            "        n %  3 == 0 ?? 'Fizz'     !! \n"
            "        n %  5 == 0 ?? 'Buzz'     !! \n"
            "        n \n"
            "    ) \n"
            ") \n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [each 20 [scope [define n [operator + it 1 ] ] [if [scope [compare == [operator % n 15 ] 0 ] ] [scope 'FizzBuzz' ] [if [scope [compare == [operator % n 3 ] 0 ] ] [scope 'Fizz' ] [if [scope [compare == [operator % n 5 ] 0 ] ] [scope 'Buzz' ] [scope n ] ] ] ] ] ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "[ 1 2 Fizz 4 Buzz Fizz 7 8 Fizz Buzz 11 Fizz 13 14 FizzBuzz 16 17 Fizz 19 Buzz ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn(
            "Foo : [ \n"
            "    x :: 10\n"
            "] \n"
            "f1 : Foo.call() \n"
            "f2 : Foo.call() \n"
            "/.co <- f1.x \n"   // ---> 10
            "/.co <- f2.x \n"   // ---> 10
        ) ;

        assert( syn.syntax == "[define Foo [list [public x 10 ] ] ] [define f1 [call Foo [scope ] ] ] [define f2 [call Foo [scope ] ] ] [push [match / co ] [match f1 x ] ] [push [match / co ] [match f2 x ] ]"
        ) ;

        TasukiInterpreter  ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "1010" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn(
            "Foo : [ \n"
            "    x2 : ( arg.0 * 2 )\n"
            "] \n"
            "f1 : Foo.call( 10 ) \n"
            "f2 : Foo.call( 20 ) \n"
            "/.co <- f1.x2 \n"   // ---> 20
            "/.co <- f2.x2 \n"   // ---> 40
        ) ;

        assert( syn.syntax == "[define Foo [list [define x2 [scope [operator * [match arg 0 ] 2 ] ] ] ] ]"
            " [define f1 [call Foo [scope 10 ] ] ]"
            " [define f2 [call Foo [scope 20 ] ] ]"
            " [push [match / co ] [match f1 x2 ] ]"
            " [push [match / co ] [match f2 x2 ] ]"
        ) ;

        TasukiInterpreter  ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "2040" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn(
            "Foo : [ \n"
            "    x : arg.0 \n"
            "    y : ( x + 1 ) \n"
            "] \n"
            "f1 : Foo.call( 10 ) \n"
            "f2 : Foo.call( 20 ) \n"
            "/.co <- f1.x \n"   // ---> 10
            "/.co <- f1.y \n"   // ---> 21
            "/.co <- f2.x \n"   // ---> 20
            "/.co <- f2.y \n"   // ---> 21
        ) ;

        assert( syn.syntax == "[define Foo [list [define x [match arg 0 ] ] [define y [scope [operator + x 1 ] ] ] ] ] "
            "[define f1 [call Foo [scope 10 ] ] ] "
            "[define f2 [call Foo [scope 20 ] ] ] "
            "[push [match / co ] [match f1 x ] ] "
            "[push [match / co ] [match f1 y ] ] "
            "[push [match / co ] [match f2 x ] ] "
            "[push [match / co ] [match f2 y ] ]"
        ) ;

        TasukiInterpreter  ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "10112021" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn(
            "--- クロージャ？ \n"
            "ClassFoo : [ \n"
            "    member : { 1 } \n"
            "    inc :: ( member <- member + 1 ) \n"
            "    dec :: ( member <- member - 1 ) \n"
            "    puts :: ( /.co <- member ) \n"
            "] \n"
            "f : ClassFoo.call() \n"
            "f.puts \n"
            "f.inc \n"
            "f.puts \n"
        ) ;

        assert( syn.syntax == "[define ClassFoo [list [define member [stream 1 ] ] "
            "[public inc [scope [push member [operator + member 1 ] ] ] ] "
            "[public dec [scope [push member [operator - member 1 ] ] ] ] "
            "[public puts [scope [push [match / co ] member ] ] ] ] "
            "] [define f [call ClassFoo [scope ] ] ] [match f puts ] [match f inc ] [match f puts ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "12" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        // 構文解析
        TasukiParseSyntax syn(
            "--- クロージャ？ \n"
            "ClassFoo : [ \n"
            "    member : { 1 } \n"
            "    inc :: ( member <- member + 1 ) \n"
            "    dec :: ( member <- member - 1 ) \n"
            "    puts :: ( /.co <- member ) \n"
            "] \n"
            "f1 : ClassFoo.call() \n"
            "f1.puts \n"
            "f1.inc \n"
            "f1.puts \n"
            "f2 : ClassFoo.call() \n"
            "f2.puts \n"
            "f2.dec \n"
            "f2.puts \n"
        ) ;

        assert( syn.syntax == "[define ClassFoo [list [define member [stream 1 ] ] [public inc [scope [push member [operator + member 1 ] ] ] ] [public dec [scope [push member [operator - member 1 ] ] ] ] [public puts [scope [push [match / co ] member ] ] ] ] ] [define f1 [call ClassFoo [scope ] ] ] [match f1 puts ] [match f1 inc ] [match f1 puts ] [define f2 [call ClassFoo [scope ] ] ] [match f2 puts ] [match f2 dec ] [match f2 puts ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "1210" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 'HelloTasuki'"
        ) ;

        assert( syn.syntax == "[push [match / co ] 'HelloTasuki' ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "HelloTasuki" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }
    
    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 1 + 2               ---> 3    --- 整数で計算されます\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [operator + 1 2 ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "3" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 1.str + 2           ---> 12   --- 文字列として連結されます\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [operator + [match 1 str ] 2 ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "12" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 1.float + 2.float   ---> 3.0  --- 実数で計算されます\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [operator + [match 1 float ] [match 2 float ] ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "3.0" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 1.0 + 2.0           ---> 3.0  --- 実数で計算されます\n"
            "/.co <- 1.5 + 2.5           ---> 4.0  --- 実数で計算されます\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [operator + [match 1 0 ] [match 2 0 ] ] ] [push [match / co ] [operator + [match 1 5 ] [match 2 5 ] ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "3.04.0" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 'Hello'         ---> Hello\n"
            "/.co <- 'Hello'.type    ---> str\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] 'Hello' ] [push [match / co ] [match 'Hello' type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "Hellostr" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- 0               ---> 0\n"
            "/.co <- 0.type          ---> int\n"
            "/.co <- 0.str.type      ---> str\n"
            "/.co <- 0.0.type        ---> float\n"
            "/.co <- 0.float.type    ---> float\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] 0 ] [push [match / co ] [match 0 type ] ] [push [match / co ] [match [match 0 str ] type ] ] [push [match / co ] [match [match 0 0 ] type ] ] [push [match / co ] [match [match 0 float ] type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "0intstrfloatfloat" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- -true             ---> -true\n"
            "/.co <- -true.type        ---> bool\n"
            "/.co <- -false            ---> -false\n"
            "/.co <- -false.type       ---> bool\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] -true ] [push [match / co ] [match -true type ] ] [push [match / co ] -false ] [push [match / co ] [match -false type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "-truebool-falsebool" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- [ ]             ---> [ ]\n"
            "/.co <- [ ].type        ---> list \n"
            "/.co <- [ 0 1 ]         ---> [ 0 1 ]\n"
            "/.co <- [ 0 1 ].type    ---> list \n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [list  ] ] [push [match / co ] [match [list  ] type ] ] [push [match / co ] [list 0 1 ] ] [push [match / co ] [match [list 0 1 ] type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "[ ]list[ ]list" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "/.co <- { }             ---> [ ]\n"
            "/.co <- { }.type        ---> stream\n"
            "/.co <- { 0 1 }         ---> 1\n"
            "/.co <- { 0 1 }.type    ---> stream\n"
        ) ;

        assert( syn.syntax == "[push [match / co ] [stream  ] ] [push [match / co ] [match [stream  ] type ] ] "
            "[push [match / co ] [stream 0 1 ] ] [push [match / co ] [match [stream 0 1 ] type ] ]" ) ;

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "[ ]stream1stream" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }


    // boss.fact : ( n : boss (if n <= 1 ?? 1 !! n * ( n - 1 ).fact ) )

    return 0 ;
}

//---------------------------------------------------------
int main()
{
    return unit_test() ;
}

