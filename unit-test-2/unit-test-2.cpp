

#include "../Tasuki-language/TasukiRun.hpp"

#include "../Tasuki-language/TasukiParse.hpp" // TasukiParseSyntax / TasukiInterpreter

#undef NDEBUG
#include <cassert> // assert()

//---------------------------------------------------------
// 単体テストコード

int unit_test()
{
    //----------------
    {
        TasukiParseSyntax syn(
            "aaa : 123        --- 123 に aaa というタグを付ける\n"
            "/.co <- aaa      --- aaa を表示してみる\n"
        ) ;    // 構文解析

        assert( syn.syntax == "[define aaa 123 ] [push [match / co ] aaa ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "123" ) ;  // 実行結果確認

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "list1 : [ aaa :: 123 ]   --- list1 というタグをつける\n"
            "/.co <- list1.aaa        --- list1 内の aaa を表示してみる\n"
            "list2 : [ aaa : 456 ]    --- list2 というタグをつける\n"
            "/.co <- list2.aaa        --- list2 内の aaa を表示してみる\n"
        ) ;    // 構文解析

        assert( syn.syntax == "[define list1 [list [public aaa 123 ] ] ] [push [match / co ] [match list1 aaa ] ] [define list2 [list [define aaa 456 ] ] ] [push [match / co ] [match list2 aaa ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

    //    assert( g_co_str == "[ ]unmatched exception. 'aaa'" ) ;  // 実行結果確認

  //      assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
//        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "list1 : [ aaa :: 123 ]\n"
            "/.co <- list1              ---> [ aaa::123 ] \n"
            "list2 : [ aaa : 456 ]\n"
            "/.co <- list2              ---> [ ] \n"
            "list3 : [ 789 ]\n"
            "/.co <- list3              ---> [ 0::789 ] \n"
        ) ;    // 構文解析

        assert( syn.syntax == "[define list1 [list [public aaa 123 ] ] ] [push [match / co ] list1 ] [define list2 [list [define aaa 456 ] ] ] [push [match / co ] list2 ] [define list3 [list 789 ] ] [push [match / co ] list3 ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

//        assert( g_co_str == "[ aaa::123 ][ ][ 0::789 ]" ) ;  // 実行結果確認

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "s : 'string'\n"
            "s : 'NG'         ----> multi define exception. 's'\n"
        ) ;    // 構文解析

        assert( syn.syntax == "[define s 'string' ] [define s 'NG' ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "" ) ;  // 実行結果確認

//        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
//        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "s : 'string'\n"
            "i : 0\n"
            "f : 1.0\n"
            "s <- 'add'  --- NG! s に push はできない\n"
            "i <- 10     --- NG! i に push はできない\n"
            "f <- 2.0    --- NG! f に push はできない\n"
        ) ;    // 構文解析

        assert( syn.syntax == "[define s 'string' ] [define i 0 ] [define f [match 1 0 ] ] [push s 'add' ] [push i 10 ] [push f [match 2 0 ] ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "" ) ;  // 実行結果確認

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    //----------------
    {
        TasukiParseSyntax syn(
            "s : { }        --- 空の strem を宣言する\n"
            "s <- 'add1'    --- OK: s には push ができる\n"
            "/.co <- s      --- add1\n"
            "s <- 'add2'    --- OK: s には push ができる\n"
            "/.co <- s      --- add2\n"
        ) ;    // 構文解析

        assert( syn.syntax == "[define s [stream  ] ] [push s 'add1' ] [push [match / co ] s ] [push s 'add2' ] [push [match / co ] s ]" ) ;

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと

        TasukiInterpreter ti( syn.syntax ) ;

        std::cout << ti.interpret() << "\n" ; // 実行

        assert( g_co_str == "add1add2" ) ;  // 実行結果確認

        assert( g_program_error_list.size() == 0 ) ; // エラーが無いこと
        assert( g_syntax_error_list .size() == 0 ) ; // エラーが無いこと
    }

    return 0 ;
}

//---------------------------------------------------------
int main()
{
    return unit_test() ;
}

