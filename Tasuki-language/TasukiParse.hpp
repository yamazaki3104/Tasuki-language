#include <fstream>
#include <iostream>
#include <iomanip>

#include <sstream>

#include <string>
#include <vector>
#include <list>
#include <map>


//---------------------------------------------------------
// Tasuki 構文解析
class TasukiParseSyntax
{
    //-----------------------------------------------------
    // 字句解析
    class ParseToken
    {
    public:
        std::vector< std::string >  token_list ;

    public:
        // コンストラクタ
        ParseToken( const std::string & _ti_code )
        {
            // debug
            std::cout << "\n"
                "input-code\n"
                "==========\n" << _ti_code << "\n";

            //---------------------------------------------------------
            // 構文解析　perse_token
            /*
                区切りの意味に使う文字（９種類）
                    ' ' スペース
                    ',' カンマ
                    ' ;' セミコロン
                    '(' 括弧
                    ' )' 閉じ括弧
                    '['
                    ']'
                    '{'
                    '}'

                文字列 str を表現するのに使う文字（３種類）
                    "'" シングルクォーテーション
                    '"' ダブルクォーテーション
                    '=' イコール

                特別な意味が割り当てられている文字（４種類）
                    ':' コロン
                    '.' ドット
                    '/' スラッシュ
                    '\' バックスラッシュ

                コメントに使う
                    '\n'
            */
            std::string  perse_token ;
            {
                // 記号の判定
                std::string  parse1 ;
                const size_t  code_max = _ti_code.size() ;
                {
                    std::vector<char> table =
                    {
                        // 区切りの意味に使う文字（９種類）
                        ' ',    // blank スペース
                        ',',    // blank カンマ
                        ';',    // blank セミコロン
                        '(',    // 括弧
                        ')',    // 閉じ括弧
                        '[',    // 括弧
                        ']',    // 閉じ括弧
                        '{',    // 括弧
                        '}',    // 閉じ括弧

                        // 文字列 str を表現するのに使う文字（３種類）
                        '\'',    // シングルクォーテーション
                        '"',    // ダブルクォーテーション
                        '=',    // イコール

                        // 特別な意味が割り当てられている文字（４種類）
                        ':',    // コロン
                        '.',    // ドット
                        '/',    // スラッシュ
                        '\\',    // バックスラッシュ

                        // コメントに使う
                        '\n'
                    } ;

                    for ( auto & j : _ti_code )
                    {{
                        char hit = 'A' ;
                        for ( auto & i : table )
                        {{
                            if ( i == j )
                            {
                                hit = i ;
                                break ; // table_max
                            }
                        }}
                        parse1 += hit ;
                    }}
                }

                // 文字列 '...' の判定
                std::string  parse2 ;
                {
                    char mode = '-' ;
                    for ( auto & j : parse1 )
                    {{
                        if (mode == 's' )
                        {
                            parse2 += 's' ;

                            if ( j == '\'' )
                                mode = '-' ;
                        }
                        else
                        {
                            if ( j == '\'' )
                            {
                                mode = 's' ;
                                parse2 += 's' ;
                            }
                            else
                            {
                                parse2 += j ;
                            }
                        }
                    }}
                }

                // 文字列 "..." の判定
                std::string  parse3 ;
                {
                    char mode = '-' ;
                    for ( int j : parse2 )
                    {{
                        if (mode == 's' )
                        {
                            parse3 += 's' ;

                            if ( j == '"' )
                                mode = '-' ;
                        }
                        else
                        {
                            if ( j == '"' )
                            {
                                mode = 's' ;
                                parse3 += 's' ;
                            }
                            else
                            {
                                parse3 += j;
                            }
                        }
                    }}
                }

                // コメント -- の判定
                {
                    char mode = '-' ;
                    for ( size_t j=0 ; j<code_max ; j++ )
                    {{
                        // '--' を探す
                        if (  j+1<code_max && parse3[j] == 'A' && _ti_code[j] == '-' && parse3[j+1] == 'A' && _ti_code[j+1] == '-' )
                            mode = 'c' ;

                        if ( parse3[j] == '\n' )
                            mode = '-' ;

                        perse_token += ( mode == 'c' ? ' ' : parse3[j] ) ;
                    }}
                }
            }

            const size_t  max = perse_token.size() ;
            std::string  token ;

            for ( size_t i=0 ; i<max ; i++ )
            {{
                const char  p = perse_token[i] ;
                if ( p == 'A' && _ti_code[i] >= 0x20 )
                { 
                    token += _ti_code[i] ;
                }
                else if ( p == 's' )
                { 
                    token += _ti_code[i] ;
                }
                else
                {
                    if ( token.size() > 0 )
                    {
                        token_list.push_back( token ) ;

                        token = "" ;
                    }
                    if ( p != ' ' && p != '\n' && _ti_code[i] >= 0x20 )
                    {
                        std::string  t ;
                        t += _ti_code[i] ;
                        token_list.push_back( t ) ;
                    }
                }
            }}
            if ( token.size() > 0 )
            {
                token_list.push_back( token ) ;
            }

            // デバッグ
            {
                std::cout << "\n"
                    "syntax-token\n"
                    "------------\n" ;

                const int max = token_list.size() ;
                for (int i = 0; i < max; i++)
                    std::cout << token_list[i] << ", ";
                std::cout << "\n";
            }
        }
    } ;

    //-----------------------------------------------------
private:
    std::vector< std::string >  syntax_list;    //

    //-----------------------------------------------------
public:
    std::string     syntax ;

    //-----------------------------------------------------
private:
    void change3_syntax(
        std::vector< std::string > &    _syntax_list,
        const std::string &             _chek,
        const std::string &             _cmd
    )
    {
        for ( int i=_syntax_list.size()-2 ; i>=1 ; i-- )
        {{
            if ( _syntax_list[i] != _chek ) continue ;

            // hit
            std::string  str = "[" + _cmd + " " + _syntax_list[i-1] + " " + _syntax_list[i+1] + " ]" ;

            _syntax_list[i-1] = str;

            const int max = _syntax_list.size() - 2;
            for ( int j=i ; j<max ; j++ )
            {{
                _syntax_list[j] = _syntax_list[j + 2] ;
            }}
            _syntax_list.pop_back() ;
            _syntax_list.pop_back() ;

            i = _syntax_list.size() - 1 ; // はじめからやりなおす
        }}
    }

    //-----------------------------------------------------
private:
    void change3_l_to_r_syntax(
        std::vector< std::string > & _syntax_list,
        const std::string &             _chek,
        const std::string &             _cmd
    )
    {
        for ( int i=1 ; i+1<_syntax_list.size() ; i++ )
        {{
            if ( _syntax_list[i] != _chek ) continue ;

            // hit
            std::string  str = "[" + _cmd + " " + _syntax_list[i-1] + " " + _syntax_list[i+1] + " ]" ;

            _syntax_list[i-1] = str;

            const int max = _syntax_list.size() - 2;
            for ( int j=i ; j<max ; j++ )
            {{
                _syntax_list[j] = _syntax_list[j + 2] ;
            }}
            _syntax_list.pop_back() ;
            _syntax_list.pop_back() ;

            i = 0 ; // はじめからやりなおす 
        }}
    }

    //-----------------------------------------------------
private:
    void change4_l_to_r_syntax(
        std::vector< std::string > & _syntax_list,
        const std::string &             _chek1,
        const std::string &             _chek2,
        const std::string &             _cmd
    )
    {
        for ( size_t i=1 ;  ; i++ )
        {{
            if ( i+3 > _syntax_list.size() ) break ;

            if ( _syntax_list[i  ] != _chek1 ) continue ;
            if ( _syntax_list[i+1] != _chek2 ) continue ;
            
            // hit
            std::string  str = "[" + _cmd + " " + _syntax_list[i - 1] + " " + _syntax_list[i + 2] + " ]";

            _syntax_list[i - 1] = str;

            const int max = _syntax_list.size() - 3;
            for ( int j=i ; j<max ; j++ )
            {
                _syntax_list[j] = _syntax_list[j + 3] ;
            }
            _syntax_list.pop_back() ;
            _syntax_list.pop_back() ;
            _syntax_list.pop_back() ;

            i = 0 ; // はじめからやりなおす
        }}
    }

    //-----------------------------------------------------
    // 演算子変換
    /*
        演算子  書き方      ---> 内部での解釈
        *       xxx * yyy   ---> [operator * xxx yyy ]
        -/-     xxx -/- yyy ---> [operator -/- xxx yyy ]
        %       xxx % yyy   ---> [operator % xxx yyy ]
        +       xxx + yyy   ---> [operator + xxx yyy ]
        -       xxx - yyy   ---> [operator - xxx yyy ]

        演算子  書き方      ---> 内部での解釈
        <       xxx < yyy   ---> [compare < xxx yyy ]
        >       xxx > yyy   ---> [compare > xxx yyy ]
        <=      xxx <= yyy  ---> [compare <= xxx yyy ]
        >=      xxx >= yyy  ---> [compare >= xxx yyy ]
        ==      xxx == yyy  ---> [compare == xxx yyy ]
        !=      xxx != yyy  ---> [compare != xxx yyy ]
        &&      xxx && yyy  ---> [&& xxx yyy ]
        ||      xxx || yyy  ---> [|| xxx yyy ]
    */
private:
    void table_check(
        std::vector< std::string > &  _syntax_list    )
    {
        change4_l_to_r_syntax( _syntax_list, ".", "call", "call" ) ;
        change4_l_to_r_syntax( _syntax_list, ".", "each", "each" ) ;
        change3_l_to_r_syntax( _syntax_list, ".", "match" ) ;

        change3_syntax( _syntax_list, "*", "operator *" ) ;
        change3_syntax( _syntax_list, "-/-", "operator -/-" ) ;
        change3_syntax( _syntax_list, "%", "operator %" ) ;
        change3_syntax( _syntax_list, "+", "operator +" ) ;
        change3_syntax( _syntax_list, "-", "operator -" ) ;

        change3_syntax( _syntax_list, "<", "compare <" ) ;
        change3_syntax( _syntax_list, ">", "compare >" ) ;
        change3_syntax( _syntax_list, "<=", "compare <=" ) ;
        change3_syntax( _syntax_list, ">=", "compare >=" ) ;
        change3_syntax( _syntax_list, "==", "compare ==" ) ;
        change3_syntax( _syntax_list, "!=", "compare !=" ) ;
        change3_syntax( _syntax_list, "&&", "&&" ) ;
        change3_syntax( _syntax_list, "||", "||" ) ;

        change3_l_to_r_syntax( _syntax_list, "<-", "push" ) ;

        change3_syntax( _syntax_list, "::", "public" ) ;
        change3_syntax( _syntax_list, ":",  "define" ) ;
    }

    //-----------------------------------------------------
    //
private:
    int find(
        const std::vector< std::string > &  _token_list,
        int                                 _start,
        const std::string &                 _t
    )
    {
        // 左から右へ '->' 確認する
        const int max = _token_list.size() ;
        for ( int i=_start ; i<max ; i++ )
        {{
            if ( _token_list[i] == _t )
                return i ; // found
        }}

        return -1 ; // NG: みつからない
    }

    //-----------------------------------------------------
    // [if [scope ... ] [scope ... ] [scope ... ] ]
    //  or
    // [scope ... ]
private:
    std::string  if_check(
        std::vector< std::string > &    _temp_syntax_list,
        int &                           _i                    // [in] 開始位置
    ) 
    {
        std::string r = "" ;

        // if文のチェック
        int  a = find( _temp_syntax_list, _i, "??" ) ;
        int  b = find( _temp_syntax_list, _i, "!!" ) ;
        if ( a >= 0 )
        {
            if ( b < 0 )
            {
                // ?? があるのに !! が無い
                g_syntax_error_list.push_back( "unmatch symbol. '!!'" ) ;
                return "" ; // Syntax error. 例外を投げたい todo
            }

            // [if [scope ... ] [scope ... ] [scope ... ] ]
            r += "[if [scope " ;
            for ( ; _i<b ; _i++ )
            {{
                if ( _i == 0 && _temp_syntax_list[0] == "if" )
                    continue ;

                if ( _i == a ) r += "] [scope " ;
                else           r += _temp_syntax_list[_i] + " " ;
            }}
            _i ++ ; // '!!'
            r += "] "  + if_check( _temp_syntax_list , _i ) + " ]" ;
        }
        else
        {
            r += "[scope " ;
            for ( ; _i<_temp_syntax_list.size() ; _i++ )
            {{
                r += _temp_syntax_list[_i] + " " ;
            }}
            r += "]" ;
        }

        return r ;
    }

    //-----------------------------------------------------
    // ( ... )
private:
    std::string  scope_check(
    )
    {
        // 右から左へ '<-' 確認する
        int   end = syntax_list.size() - 1 ;
        int   from = end ;
        bool  hit = false ;
        for ( ; from>=0 ; from-- )
        {{
            if ( syntax_list[from] == "(" )
            {
                hit = true ; break ;
            }
        }}
        if ( hit == false )
        {
            g_syntax_error_list.push_back( "unmatch symbol. '('" ) ;
            return "" ; // Syntax error
        }

        // 切り出し
        std::vector< std::string >  temp_syntax_list;
        
        for ( int i=from+1 ; i<=end ; i++ )
        {{
            temp_syntax_list.push_back( syntax_list[i] ) ;
        }}

        for ( int i=from ; i<=end ; i++ )
        {{
            syntax_list.pop_back() ;
        }}

        table_check( temp_syntax_list ) ; // if_check() のあと table_check() がただしいはず todo

        // ここですべて変換されているはず

        int i = 0 ;
        return if_check( temp_syntax_list, i ) ;
    }

    //-----------------------------------------------------
    // [ ... ]
private:
    std::string  list_check(
    )
    {
        // 右から左へ '<-' 確認する
        int   end = syntax_list.size() - 1 ;
        int   from = end ;
        bool  hit = false ;
        for ( ; from>=0 ; from-- )
        {{
            if ( syntax_list[from] == "[" )
            {
                hit = true ; break ;
            }
        }}
        if ( hit == false )
        {
            g_syntax_error_list.push_back( "unmatch symbol. '['" ) ;
            return "" ; // Syntax error
        }

        // 切り出し
        std::vector< std::string >  temp_syntax_list;
        
        for ( int i=from+1 ; i<=end ; i++ )
        {{
            temp_syntax_list.push_back( syntax_list[i] ) ;
        }}

        for ( int i=from ; i<=end ; i++ )
        {{
            syntax_list.pop_back() ;
        }}

        table_check( temp_syntax_list ) ;

        // ここですべて変換されているはず
        std::string r = "[list " ;
        for ( size_t i=0 ; i<temp_syntax_list.size() ; i++ )
        {{
            r += temp_syntax_list[i] ;

            if ( i < temp_syntax_list.size() - 1 ) // 最後に空白はつけない
                r += " " ;
        }}
        r += " ]" ;

        return r ;
    }

    //-----------------------------------------------------
    // { ... }
private:
    std::string  stream_check(
    )
    {
        // 右から左へ '<-' 確認する
        int   end = syntax_list.size() - 1 ;
        int   from = end ;
        bool  hit = false ;
        for ( ; from>=0 ; from-- )
        {{
            if ( syntax_list[from] == "{" )
            {
                hit = true ; break ;
            }
        }}
        if ( hit == false )
        {
            g_syntax_error_list.push_back( "unmatch symbol. '{'" ) ;
            return "" ; // Syntax error
        }

        // 切り出し
        std::vector< std::string >  temp_syntax_list;
        
        for ( int i=from+1 ; i<=end ; i++ )
        {{
            temp_syntax_list.push_back( syntax_list[i] ) ;
        }}

        for ( int i=from ; i<=end ; i++ )
        {{
            syntax_list.pop_back() ;
        }}

        table_check( temp_syntax_list ) ;

        // ここですべて変換されているはず
        std::string r = "[stream " ;
        for ( size_t i=0 ; i<temp_syntax_list.size() ; i++ )
        {{
            r += temp_syntax_list[i] ;

            if ( i < temp_syntax_list.size() - 1 ) // 最後に空白はつけない
                r += " " ;
        }}
        r += " ]" ;

        return r ;
    }

    //-----------------------------------------------------
    // /... -/
private:
    std::string  control_check(
    )
    {
        // 右から左へ '<-' 確認する
        int   end = syntax_list.size() - 1 ;
        int   from = end ;
        bool  hit = false ;
        for ( ; from>=0 ; from-- )
        {{
            if ( syntax_list[from] == "/" )
            {
                hit = true ; break ;
            }
        }}
        if ( hit == false )
        {
            g_syntax_error_list.push_back( "unmatch symbol. '/'" ) ;
            return "" ; // Syntax error
        }

        // 切り出し
        std::string r = "[" + syntax_list[ from+1 ] + " " ;

        if ( r != "[break "
          && r != "[continue "
          && r != "[skip "
          && r != "[repeat "
          && r != "[exception "
            )
        {
            g_syntax_error_list.push_back( "unknown symbol. '" + syntax_list[from + 1] + "'" ) ;
            return "" ; // Syntax error  知らない（サポートしない）文字列が来た
        }

        std::vector< std::string >  arg_syntax_list ;
        std::vector< std::string >  to_syntax_list ;
        std::vector< std::string >  if_syntax_list ;

        std::string mode = "arg" ; // "to", "if"
        for ( int i=from+2 ; i<=end-1 ; i++ )
        {{
            if ( syntax_list[i] == "to" )
            {
                mode = "to" ;
                continue ;
            }
            if ( syntax_list[i] == "if" )
            {
                mode = "if" ;
                continue ;
            }

            if ( mode == "arg" )
            {
                arg_syntax_list.push_back( syntax_list[i] ) ;
            }
            else if ( mode == "to" )
            {
                to_syntax_list.push_back( syntax_list[i] ) ;
            }
            else if ( mode == "if" )
            {
                if_syntax_list.push_back( syntax_list[i] ) ;
            }
            else
                PROGRAM_ERROR( "mode error." ) ; // ここには来ないはず
        }}

        for ( int i=from ; i<=end ; i++ )
        {{
            syntax_list.pop_back() ;
        }}

        // 引数部分
        {
            table_check( arg_syntax_list ) ;

            if ( r == "[break "
              || r == "[continue "
              || r == "[exception "
                )
            {
                r += "[list " ;
            }
            else // "[skip ", "[repeat "
            {
                r += "[scope " ;
            }

            for ( auto & it : arg_syntax_list )
            {{
                r += it + " " ;
            }}
            r += "] " ;
        }

        // to
        {
            if ( to_syntax_list.size() <= 1 )
            {
                if ( to_syntax_list.size() == 1 )
                {
                    r += to_syntax_list[0] + " " ;
                }
            }
            else
                g_syntax_error_list.push_back( "warning. too many args. 'to'" ) ; // 'to' の引数が多すぎる

            r += "]" ;
        }

        // if文のチェック
        if ( if_syntax_list.size() >= 1 )
        {
            table_check( if_syntax_list ) ;

            std::string rr ;
            for ( auto & it : if_syntax_list )
            {{
                rr += it + " " ;
            }}

            return "[if [scope " + rr + "] " + r + " [scope ] ]" ;
        }

        return r ;
    }

    //-----------------------------------------------------
    // Tasuki 構文解析（コンストラクタ）
public:
    TasukiParseSyntax(
        const std::string  & _tasuki_code
    )
    {
        g_program_error_list.clear() ;      //
        g_syntax_error_list.clear() ;       //

        // 字句解析
        ParseToken  tl( _tasuki_code ) ;

        // 左から右へ '->' 確認する
        const int max = tl.token_list.size() ;
        for ( int i=0 ; i<max ; i++ )
        {{
            if ( tl.token_list[i] == ")" )
            {
                // todo メソッドコールのチェック

                syntax_list.push_back( scope_check() ) ;
            }
            else if ( tl.token_list[i] == "]" )
            {
                syntax_list.push_back( list_check() ) ;
            }
            else if ( tl.token_list[i] == "}" )
            {
                syntax_list.push_back( stream_check() ) ;
            }
            else if ( tl.token_list[i] == "/" && syntax_list.size()>0 && syntax_list[syntax_list.size() - 1] == "-" ) // 制御文
            {
                syntax_list.push_back( control_check() ) ;
            }
            else
            {
                auto tkn = tl.token_list[ i ] ;

                int last = syntax_list.size() ;
                if ( last > 0 )
                {
                    // ２文字以上の演算子を連結して再登録
                    auto &  lst = syntax_list[ last - 1 ] ;
                    if ( lst == "<" && tkn == "-" ){ lst = "<-" ; continue ; }
                    if ( lst == "<" && tkn == "=" ){ lst = "<=" ; continue ; }
                    if ( lst == ">" && tkn == "=" ){ lst = ">=" ; continue ; }
                    if ( lst == "=" && tkn == "=" ){ lst = "==" ; continue ; }
                    if ( lst == "!" && tkn == "=" ){ lst = "!=" ; continue ; }
                    if ( lst == ":" && tkn == ":" ){ lst = "::" ; continue ; }
                    if ( lst == ">" && tkn == ">" ){ lst = ">>" ; continue ; }
                    if ( lst == "<" && tkn == "<" ){ lst = "<<" ; continue ; }
                    if ( lst == "-" && tkn == ">" ){ lst = "->" ; continue ; }
                    if ( lst == "->" && tkn == ">" ){ lst = "->>" ; continue ; }
                }
                syntax_list.push_back( tkn ) ;
            }
        }}

        // 演算子を変換
        table_check( syntax_list ) ;

        for ( int i=0 ; i<syntax_list.size() ; i++ )
        {{
            syntax += syntax_list[i] ;

            if ( i < syntax_list.size() - 1 ) // 最後に空白はつけない
                syntax += " " ;
        }}

        // debug
        std::cout << "\n"
            "syntax-parse\n"
            "------------\n" << syntax << "\n";
    }
};

//---------------------------------------------------------
// 実行コード
std::map< std::string, std::vector< std::string > >  g_t_code_list ;

// 実行（評価）
int t_code_evaluation(
    const std::string & _t_code
)
{
    std::string str = _t_code ;
    int idx = -1 ;

    if ( str.substr( 0, 2 ) != "t:" )
        return -1 ; // OK

    for ( int i=0 ; i<99 ; i++ )
    {{
        // debug print
        {
            std::cout << str << " { " ;
            for ( auto & item : g_t_code_list[ str ] )
            {{
                std::cout << "'" << item << "', " ;
            }}
            std::cout << "}\n" ;
        }

        // define の場合、継続せずに、defineだけして終了する。（ってか defineの戻り値が t_dode なのが問題？？）
        if ( g_t_code_list[ str ][0] == "define"
          || g_t_code_list[ str ][0] == "public"
           )
        {
            int i = run( g_t_code_list[ str ] ) ; // OK defineだけして終了する

            // debug
            Value & v = g_stack.get_idx_value( i ) ; // 実行結果
            std::cout << "\t" << g_t_code_list[ str ][0] << " -> " << v.m_idx << ":" << v.m_type << " " << v.m_str << "\n" ;
            return i ;
        }


        idx = run( g_t_code_list[ str ] ) ; // 実行
        if ( idx < 0 )
            return idx ;

        Value & v = g_stack.get_idx_value( idx ) ; // 実行結果

        {
            // debug
            std::cout << "\t"  << g_t_code_list[ str ][0] << " -> " << v.m_idx << ":" << v.m_type << " " << v.m_str << "\n" ;
        }

        if ( v.m_type != "t_code" )
            return idx ; // OK

        str = v.m_to_be_evaluated_list[0] ;

        if ( str.substr( 0, 2 ) != "t:" ) return idx ; // OK
    }}

    return PROGRAM_ERROR( "limit over." ) ;
}

//---------------------------------------------------------
class TasukiInterpreter
{
    //---------------------------------------------------------
    // Tasuki 字句解析
    class TokenList
    {
    public:
        std::vector< std::string >  token_list;
        std::vector< std::string >  type_list;

    public:
        // コンストラクタ
        TokenList( const std::string & _ti_code )
        {
            //---------------------------------------------------------
            // 構文解析　perse_token
            std::string  perse_token ;
            {
                // 記号の判定
                std::string  parse1 ;
                const int  code_max = _ti_code.size() ;
                {
                    std::vector<char> table =
                    {
                        // 区切りの意味に使う文字
                        ' ',    // blank スペース
                        '[',    // 括弧
                        ']',    // 閉じ括弧

                        // 文字列 str を表現するのに使う文字
                        '\'',    // シングルクォーテーション
                        '"',    // ダブルクォーテーション
                    } ;

                    for ( auto & j : _ti_code )
                    {{
                        char hit = 'A' ;
                        for ( auto & i : table )
                        {{
                            if ( i == j )
                            {
                                hit = i ;
                                break ; // table_max
                            }
                        }}
                        parse1 += hit ;
                    }}
                }

                // 文字列 '...' の判定
                std::string  parse2 ;
                {
                    char mode = '-' ;
                    for ( auto & j : parse1 )
                    {{
                        if (mode == 's' )
                        {
                            parse2 += 's' ;

                            if ( j == '\'' )
                                mode = '-' ;
                        }
                        else
                        {
                            if ( j == '\'' )
                            {
                                mode = 's' ;
                                parse2 += 's' ;
                            }
                            else
                            {
                                parse2 += j ;
                            }
                        }
                    }}
                }

                // 文字列 "..." の判定
                {
                    char mode = '-' ;
                    for ( int j : parse2 )
                    {{
                        if (mode == 's' )
                        {
                            perse_token += 's' ;

                            if ( j == '"' )
                                mode = '-' ;
                        }
                        else
                        {
                            if ( j == '"' )
                            {
                                mode = 's' ;
                                perse_token += 's' ;
                            }
                            else
                            {
                                perse_token += j;
                            }
                        }
                    }}
                }
            }

            const size_t  max = perse_token.size() ;
            std::string  token ;
            std::string  type = "type_symbol" ;

            for ( size_t i=0 ; i<max ; i++ )
            {{
                const char  p = perse_token[i] ;
                if ( p == 'A' && _ti_code[i] >= 0x20 )
                { 
                    token += _ti_code[i] ;
                    type = "type_alphabet" ; // 英字
                }
                else if ( p == 's' )
                { 
                    token += _ti_code[i] ;
                    type = "type_string" ; // 文字列
                }
                else
                {
                    if ( token.size() > 0 )
                    {
                        token_list.push_back( token ) ;
                        type_list.push_back( type ) ;

                        token = "" ;
                    }
                    if ( p != ' ' && p != '\n' && _ti_code[i] >= 0x20 )
                    {
                        std::string  t ;
                        t += _ti_code[i] ;
                        token_list.push_back( t ) ;
                        type_list.push_back( "type_symbol" ) ; // 記号
                    }
                }
            }}
            if ( token.size() > 0 )
            {
                token_list.push_back( token ) ;
                type_list.push_back( type ) ;
            }

            // デバッグ
            {
                std::cout << "\n"
                    "interpreter-token\n"
                    "-----------------\n" ;

                const int max = token_list.size() ;
                for (int i = 0; i < max; i++)
                    std::cout << token_list[i] << ", ";
                std::cout << "\n";
            }
        }
    } ;

    //-----------------------------------------------------
    // Tasuki インタープリタ（コンストラクタ）
public:
    TasukiInterpreter(
        const std::string  & _tasuki_code
    )
    {
        g_t_code_list.clear() ;

        // 字句解析
        TokenList  tl( "[scope " + _tasuki_code + " ]" ) ;

        // std::list にコピー
        std::list< std::string >  token_list ;
        for ( auto & it : tl.token_list )
        {{
            token_list.push_back( it ) ;
        }}

        int  count = 0 ;

        // [ ... ']' 左から右へ '->' 確認する
        for ( auto it=token_list.begin() ; it!=token_list.end() ; it++ )
        {{
            if ( *it != "]" ) continue ;

            std::list< std::string > n ;

            // '[' ... ] 右から左へ '<-' 確認する
            for ( ; /*it != token_list.begin()*/ ; )
            {{
                n.push_front( *it ) ;
                auto a = it;

                if ( *a != "[" )
                {
                    it -- ;
                    token_list.erase( a ) ;
                    continue ;
                }

                // hit
                std::vector< std::string > temp ;
                for ( auto & item : n )
                {{
                    if ( item == "[" ) continue ;
                    if ( item == "]" ) continue ;

                    temp.push_back( item ) ;
                }}
                std::stringstream   no ;
                no << "t:" << std::setfill('0') << std::setw(3) << count++ ;

                g_t_code_list[ no.str() ] = temp ;

                it ++ ;
                token_list.insert( it, no.str() ) ;
                token_list.erase( a ) ;
                it -- ;

                break ;
            }}
        }}

        // debug
        std::cout << "\n"
            "interpreter-code\n"
            "----------------\n" ;

        for ( auto & a : g_t_code_list )
        {{
            std::cout << a.first << ": { " ;
            for ( auto & item : a.second )
            {{
                std::cout << "\"" << item << "\", " ;
            }}
            std::cout << "} ;\n" ;
        }}
    }

    //-----------------------------------------------------
    // 実行（インタープリター）
    std::string  interpret()
    {
        // グローバル領域のクリア
        g_stack.clear() ;
        g_program_error_list.clear() ;
        g_syntax_error_list.clear() ;
        g_co_str.clear() ;

        std::cout << "\n"
            "interpreter-run\n"
            "---------------\n" ;

        // 最後の「行」がmainなので、最後の行を実行する
        std::stringstream  no;
        no << std::setfill('0') << std::setw(3) << g_t_code_list.size()-1 ;

        int idx = run( g_t_code_list[ "t:" + no.str() ] ) ;

        if ( idx < 0 )
            return "NG" ;

        return g_stack.get_idx_value( idx ).m_str ;
    }
} ;
