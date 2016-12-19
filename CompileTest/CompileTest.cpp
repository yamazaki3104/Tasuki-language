
// コンパイラと呼ぶにはおこがましいが「こんな感じで実行形式 exe も作れますよー」という提案
// ただ、このコードを出力するコードはまだない ^^;
// っていうか、もっと template とかを駆使してがっつり最適化されるようなコードにしたい。

#include "../Tasuki-language/TasukiRun.hpp"

/*
factorial のコード↓を t_code 中間言語？に翻訳して展開。

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
*/

int f0()  { std::vector< std::string > t = { "match", "arg", "0"             } ; return run( t ) ; } ;
int f1()  { std::vector< std::string > t = { "define", "n", "t:0"            } ; return run( t ) ; } ;
int f2()  { std::vector< std::string > t = { "compare", "<=", "n", "1"       } ; return run( t ) ; } ;
int f3()  { std::vector< std::string > t = { "scope", "t:2"                  } ; return run( t ) ; } ;
int f4()  { std::vector< std::string > t = { "scope", "1"                    } ; return run( t ) ; } ;
int f5()  { std::vector< std::string > t = { "operator", "-", "n", "1"       } ; return run( t ) ; } ;
int f6()  { std::vector< std::string > t = { "scope", "t:5"                  } ; return run( t ) ; } ;
int f7()  { std::vector< std::string > t = { "call", "factorial", "t:6"      } ; return run( t ) ; } ;
int f8()  { std::vector< std::string > t = { "operator", "*", "n", "t:7"     } ; return run( t ) ; } ;
int f9()  { std::vector< std::string > t = { "scope", "t:8"                  } ; return run( t ) ; } ;
int f10() { std::vector< std::string > t = { "if", "t:3", "t:4", "t:9"       } ; return run( t ) ; } ;
int f11() { std::vector< std::string > t = { "scope", "t:1", "t:10"          } ; return run( t ) ; } ;
int f12() { std::vector< std::string > t = { "define", "factorial", "t:11"   } ; return run( t ) ; } ;
int f13() { std::vector< std::string > t = { "match", "/", "co"              } ; return run( t ) ; } ;
int f14() { std::vector< std::string > t = { "scope", "3"                    } ; return run( t ) ; } ;
int f15() { std::vector< std::string > t = { "call", "factorial", "t:14"     } ; return run( t ) ; } ;
int f16() { std::vector< std::string > t = { "push", "t:13", "t:15"          } ; return run( t ) ; } ;
int f17() { std::vector< std::string > t = { "match", "/", "co"              } ; return run( t ) ; } ;
int f18() { std::vector< std::string > t = { "scope", "6"                    } ; return run( t ) ; } ;
int f19() { std::vector< std::string > t = { "call", "factorial", "t:18"     } ; return run( t ) ; } ;
int f20() { std::vector< std::string > t = { "push", "t:17", "t:19"          } ; return run( t ) ; } ;
int f21() { std::vector< std::string > t = { "scope", "t:12", "t:16", "t:20" } ; return run( t ) ; } ;


std::map< std::string, int (*)() > g_function_define_table;

int  t_code_evaluation(
    const std::string & _t_code
)
{
    if ( _t_code.size() < 2 || _t_code[0] != 't' || _t_code[1] != ':' )
        return -1 ; // t_code 以外

    if ( g_function_define_table.count( _t_code ) != 1 )
        return -1 ; // みつからない "program error." ;

    return g_function_define_table[ _t_code ]() ; // t_code は実行する
}


int main()
{
    g_function_define_table["t:0"] = f0 ;
    g_function_define_table["t:1"] = f1 ;
    g_function_define_table["t:2"] = f2 ;
    g_function_define_table["t:3"] = f3 ;
    g_function_define_table["t:4"] = f4 ;
    g_function_define_table["t:5"] = f5 ;
    g_function_define_table["t:6"] = f6 ;
    g_function_define_table["t:7"] = f7 ;
    g_function_define_table["t:8"] = f8 ;
    g_function_define_table["t:9"] = f9 ;
    g_function_define_table["t:10"] = f10 ;
    g_function_define_table["t:11"] = f11 ;
    g_function_define_table["t:12"] = f12 ;
    g_function_define_table["t:13"] = f13 ;
    g_function_define_table["t:14"] = f14 ;
    g_function_define_table["t:15"] = f15 ;
    g_function_define_table["t:16"] = f16 ;
    g_function_define_table["t:17"] = f17 ;
    g_function_define_table["t:18"] = f18 ;
    g_function_define_table["t:19"] = f19 ;
    g_function_define_table["t:20"] = f20 ;
    g_function_define_table["t:21"] = f21 ;

    return f21() ;
}

