
// /Tasuki -/ 実行環境。インタープリタだけでなく、コンパイルにも使えるように書いたつもり、、、

#include <fstream>
#include <iostream>
#include <iomanip>

#include <sstream>

#include <string>
#include <vector>
#include <list>

#include <map>

#include <iostream> // std::cout

//---------------------------------------------------------

#define PROGRAM_ERROR( _MSG ) [&](){ std::stringstream ss ; ss << _MSG << "\nprogram error.\nfile: " << __FILE__ << "\nline: " << __LINE__ << "\nfunction: " << __FUNCTION__ << "\n" ; g_program_error_list.push_back( ss.str() ) ; return -1 ; }()

#define RUNTIME_ERROR( _MSG ) [&](){ std::stringstream ss ; ss << _MSG << "\nrun-time error.\nfile: " << __FILE__ << "\nline: " << __LINE__ << "\nfunction: " << __FUNCTION__ << "\n" ; g_syntax_error_list.push_back( ss.str() ) ; return -1 ; }()

std::vector< std::string >  g_program_error_list ;      // 
std::vector< std::string >  g_syntax_error_list ;       // 

std::string                 g_co_str ; // 単体テスト用、標準出力


//---------------------------------------------------------
// 
class Value ;

int  t_code_evaluation( const std::string & _t_code ) ; // t_code を評価する関数

Value & g_stack_get_idx_value( int _idx ) ;

int g_stack_get_boss_cd( int _i ) ;

Value &  g_stack_alloc_value() ;

// t_code やリテラル文字を Value にして、そのindexを返す
int  t_code_to_value( const std::string & _t_code ) ;

// 
class Value
{
public:
    std::string d ; // デバッグ用なので、リリース時には消します

    int     m_idx ; // g_stack内の自分の位置

    int     m_boss ;   // 所属チームの管理者

    //-----------------------------------------------------
public:
    std::string     m_type ;   // 'str', 'int', 'list', 'stream'
    std::string     m_str ;    // 'str' のときに値が入る

    int             m_num ;    // 'int' のときに値が入る

    // list
    std::vector< std::string >  m_to_be_evaluated_list ; // 'list' の場合、未評価値 t_code が入る

    std::vector< int >          m_evaluated_index ;     // 評価済みの Value へのポインタ（インデックス）

    // define
    std::map< std::string, int >  m_private_define_map ; // m_evaluated_value_listの登録位置を記録
    std::map< std::string, int >  m_public_define_map ;  // m_evaluated_value_listの登録位置を記録


    //-----------------------------------------------------
    // 未評価( t_code )のままで止まっている m_to_be_evaluated_list を評価する（値を置き換える）
public:
    int eval()
    {
        for ( int i=0 ; i<99 ; ++i ) // todo 安定して動いてきたら無限ループに変える（無限ループ回避用）
        {{
            if ( m_type == "stream" )
            {
                // todo ほんとはたくさんあって、最後の値を set するべき
                if ( m_to_be_evaluated_list.size() != 1 )
                    return -1 ; // "program_error!!." ;

                int idx = t_code_to_value( m_to_be_evaluated_list[0] ) ;
                if ( idx < 0 ) return idx ; // NG

                Value & v = g_stack_get_idx_value( idx ) ;

                v.find_tag() ; // unknown_type の場合は tag_search() して置き換える

                int tmp_idx = this->m_idx ;
                *this = v ; // copy
                this->m_idx = tmp_idx ;

                m_type = "stream" ; // 試しにちょっとここで耐えてもらって、、、 todo

                return idx ;
            }

            if ( m_type != "t_code" )
                return m_idx ;

            if ( m_to_be_evaluated_list.size() != 1 )
                return PROGRAM_ERROR( "t_code not found." ) ; // "program_error!!." ;

            int idx = t_code_to_value( m_to_be_evaluated_list[0] ) ;
            if ( idx < 0 ) return idx ; // NG

            Value & v = g_stack_get_idx_value( idx ) ;

            v.find_tag() ; // unknown_type の場合は tag_search() して置き換える

            int tmp_idx = this->m_idx ;
            *this = v ; // copy
            this->m_idx = tmp_idx ;
        }}

        return PROGRAM_ERROR( "loop limit over." ) ; // "program_error!!." ;
    }

    //-----------------------------------------------------
    // 
public:
    int tag_search( const std::string & _str )
    {
        auto & m = m_public_define_map ;

        if ( m.count( _str ) == 1 )
            return m[_str] ;

        if ( m_boss < 0 )
            return -1 ; // 見つからなかった

        if ( g_stack_get_idx_value( m_boss ).m_boss == m_boss )
            return PROGRAM_ERROR( "tree-structure is broken." ) ; // program error  木構造の構築に失敗している！

        return g_stack_get_idx_value( m_boss ).tag_search( _str ) ;
    }

    //-----------------------------------------------------
    // 
public:
    bool find_tag()
    {
        if ( m_type != "unknown_type" ) return true ; // OK

        for ( int i=0 ; i<99 ; i++ )
        {{
            int boss_idx = g_stack_get_boss_cd( i ) ;
            if ( boss_idx < 0 )
                return false ; // NG

            Value & boss = g_stack_get_idx_value( boss_idx ) ;

            int idx = boss.tag_search( m_str ) ; // タグを探る

            if ( idx >= 0 )
            {
                // HIT
                Value & f = g_stack_get_idx_value( idx ) ;

                // m_idx 以外を自分にコピーする
                int tmp_idx = m_idx ;
                *this = f ; // copy
                m_idx = tmp_idx ;

                return true ; // OK
            }
        }}

        return false ; // NG
    }

    //-----------------------------------------------------
    // コンストラクタ
public:
    Value( int _boss = -1 )
    : m_type( "unknown" )
    , m_num( 0 )
    , m_boss( _boss )
    {
    }
} ;

//---------------------------------------------------------
int find_tag(
    const std::string &  _tag_name
)
{
    for ( int i=0 ; i<99 ; i++ )
    {{
        int boss_idx = g_stack_get_boss_cd( i ) ;
        if ( boss_idx < 0 )
            return -1 ; // NG

        Value & boss = g_stack_get_idx_value( boss_idx ) ;

        int idx = boss.tag_search( _tag_name ) ; // タグを探る

        if ( idx >= 0 )
        {
            // HIT
            return idx ; // OK
        }
    }}

    return -1 ; // NG
}

//---------------------------------------------------------
// t_code やリテラル文字を Value にして、そのindexを返す
int  t_code_to_value(
    const std::string &  _t_code // [in] t_code やリテラル文字
)
{
    // t_code 't:' は評価する
    {
        int idx = t_code_evaluation( _t_code ) ;
        if ( idx >= 0 ) // 評価
            return idx ;
    }

    // 数字(int)かどうか？
    int num = 0 ;
    if ( sscanf_s( _t_code.c_str(), "%d", &num ) == 1 )
    {
        Value & v = g_stack_alloc_value() ; // あたらしい領域を確保

        v.m_type = "int" ;
        v.m_str  = _t_code ;
        v.m_num  = num ;

        v.d += "[" + v.m_str + ":rtn-value]" ; // todo

        return v.m_idx ; // OK
    }

    // 数字(int)ではないなら、文字(str)か？タグ名か
    if ( _t_code.size() >= 2 && _t_code[0]=='\'' && _t_code[_t_code.size()-1]=='\'' )
    {
        Value & v = g_stack_alloc_value() ; // あたらしい領域を確保

        v.m_type = "str" ;
        v.m_str  = _t_code.substr( 1, _t_code.size()-2 ) ;

        v.d += "[" + v.m_str + ":rtn-value]" ; // todo

        return v.m_idx ; // OK
    }

    if ( _t_code == "-true" || _t_code == "-false" )
    {
        Value & v = g_stack_alloc_value() ; // あたらしい領域を確保

        v.m_type = "bool" ;
        v.m_str  = _t_code ;

        v.d += "[" + v.m_str + ":rtn-value]" ; // todo

        return v.m_idx ; // OK
    }

    // 知らない型 これは領域を確保しなくていいと思った。
    Value & v = g_stack_alloc_value() ; // あたらしい領域を確保

    v.m_type = "unknown_type" ;
    v.m_str  = _t_code ;

    v.d += "[" + v.m_str + ":rtn-value]" ; // todo

    return v.m_idx ; // OK
}

std::string  check_value_type(
    const std::string &  _t_code // [in] t_code やリテラル文字
)
{
    // t_code 't:' は評価する
    if ( _t_code.size() >= 2 && _t_code[0]=='t' && _t_code[1]==':' )
    {
        return "t_code" ;
    }

    // 数字(int)かどうか？
    int num = 0 ;
    if ( sscanf_s( _t_code.c_str(), "%d", &num ) == 1 )
    {
        return "int" ;
    }

    // 数字(int)ではないなら、文字(str)か？タグ名か
    if ( _t_code.size() >= 2 && _t_code[0]=='\'' && _t_code[_t_code.size()-1]=='\'' )
    {
        return "str" ;
    }

    if ( _t_code == "-true" || _t_code == "-false" )
    {
        return "bool" ;
    }

    // 知らない型
    return "unknown_type" ;
}


//---------------------------------------------------------
// スタック領域
class Stack
{
private:
    std::list< Value >      m_evaluated_value_list ; // 評価済みの値が入る
    std::vector< size_t >   m_evaluated_value_list_size_stack ;

    std::vector< int >      m_boss_list ; // 登録されるべきチームの管理者

public:
    std::vector< int >      m_boss_cd ; // メンバを探すときに問い合わせるチームの管理者（カレントディレクトリ）

    //-----------------------------------------------------
public:
    Stack()
    {
        clear() ;
    }

    //-----------------------------------------------------
public:
    void clear()
    {
        m_evaluated_value_list.clear() ;
        m_evaluated_value_list_size_stack.clear() ;
        m_boss_list.clear() ;
        m_boss_list.push_back( -1 ) ;
        m_boss_cd.clear() ;
        m_boss_cd.push_back( -1 ) ;
    }

    //-----------------------------------------------------
public:
    void push_boss()
    {
        m_boss_list.push_back( get_index() ) ;
        m_boss_cd.push_back( get_index() ) ;
    }

    //-----------------------------------------------------
public:
    int get_boss_index()
    {
        int sz = m_boss_list.size() ;

        return m_boss_list[ sz -1 ] ;
    }

    //-----------------------------------------------------
public:
    void pop_boss()
    {
        m_boss_list.pop_back() ;
        m_boss_cd.pop_back() ;
    }

    //-----------------------------------------------------
public:
    void debug_print()
    {
        std::cout << ">> m_evaluated_value_list:" << m_evaluated_value_list.size() << "\n" ;
        for ( auto & it : m_evaluated_value_list )
        {{
            std::cout << ">> " << it.m_idx
                << ", boss:" << it.m_boss
                << ", type:" << it.m_type
                << ", str:" << it.m_str
                << "\n" ;
        }}
    }

    //-----------------------------------------------------
public:
    void push()
    {
        m_evaluated_value_list_size_stack.push_back( m_evaluated_value_list.size() ) ; // 評価済みの値が入る
    }

    //-----------------------------------------------------
public:
    void pop()
    {
        if ( m_evaluated_value_list_size_stack.size() == 0 )
            return ; // 限りなくプログラムエラーに近い

        auto sz = m_evaluated_value_list_size_stack[m_evaluated_value_list_size_stack.size()-1] ;

        m_evaluated_value_list.resize( sz ) ; // 評価済みの値が入るlist

        m_evaluated_value_list_size_stack.pop_back() ;
    }

    //-----------------------------------------------------
public:
    Value & alloc_value()
    {
        //        if ( m_evaluated_value_list.size() > 99 )
        //            return RUNTIME_ERROR( "stack overflow" ) ; // "NG: call stack overflow" ;

        Value v( get_boss_index() ) ;

        v.m_idx = m_evaluated_value_list.size() ;

        m_evaluated_value_list.push_back( v ) ;

        return get_value() ;
    }


    //-----------------------------------------------------
public:
    Value & get_value()
    {
        return m_evaluated_value_list.back() ;
    }

    //-----------------------------------------------------
public:
    int get_index()
    {
        return m_evaluated_value_list.size()-1 ;
    }

    //-----------------------------------------------------
public:
    Value & get_idx_value( int _idx )
    {
        if ( _idx < 0 )
        {
            PROGRAM_ERROR( "g_stack idx < 0" ) ; 
            return *m_evaluated_value_list.begin() ; // dummy
        }
        if ( _idx >= m_evaluated_value_list.size() )
        {
            PROGRAM_ERROR( "g_stack idx >= size" ) ; 
            return *m_evaluated_value_list.begin() ; // dummy
        }

        auto & it = m_evaluated_value_list.begin() ;
        for ( int i=0 ; i<_idx ; ++i ) ++it ;

        return *it ;
    }

    //-----------------------------------------------------
    // define は登録のみで、評価はしない
public:
    int public_define( const std::string & _name, const std::string & _f )
    {
        Value & f = alloc_value() ;

        f.m_type = "t_code" ;
        f.m_str = _name +"->"+_f+" : to_be_evaluated" ;  // デバッグ用なのでリリース時には消します todo
        f.m_to_be_evaluated_list.push_back( _f ) ;

        f.d += "[" + _name + ":define]" ;  // デバッグ用なのでリリース時には消します todo

        // boss に f を登録する
        Value & boss = get_idx_value( get_boss_index() ) ;

        auto & def_map = boss.m_public_define_map ; 

        // すでに define 済みの場合、例外を投げる todo
        if ( def_map.count( _name ) != 0 )
            return RUNTIME_ERROR( "define:" << _name << "->" << _f ) ; // "NG: runtime-error. define" ;

        def_map[ _name ] = f.m_idx ; // boss に f を登録する

        boss.d +=  "[" + _name + ":add-def]" ;  // デバッグ用なのでリリース時には消します todo

        return f.m_idx ;
    }

    //-----------------------------------------------------
public:
    int private_define( const std::string & _name, const std::string & _f )
    {
        Value & f = alloc_value() ;

        f.m_type = "t_code" ;
        f.m_str = _name +"->"+_f+" : to_be_evaluated" ;
        f.m_to_be_evaluated_list.push_back( _f ) ;

        Value & boss = get_idx_value( get_boss_index() ) ;

        auto & def_map = boss.m_private_define_map ; 

        // すでに define 済みの場合、例外を投げる todo
        if ( def_map.count( _name ) != 0 )
            return RUNTIME_ERROR( "define:" << _name << "->" << _f ) ; // "NG: runtime-error. define" ;

        def_map[ _name ] = f.m_idx ; // boss に f を登録する

        boss.d +=  "[" + _name + ":add-def]" ;  // デバッグ用なのでリリース時には消します todo

        return f.m_idx ;
    }
} ;

Stack g_stack ;


Value & g_stack_get_idx_value( int _idx )
{
    return g_stack.get_idx_value( _idx ) ;
}

int g_stack_get_boss_cd( int _i )
{
    int idx = g_stack.m_boss_cd.size() - 1 - _i ;

    if ( idx < 0 ) return -1 ;

    return g_stack.m_boss_cd[ idx ] ;
}

Value & g_stack_alloc_value()
{
    return g_stack.alloc_value() ; // あたらしい領域を確保
}

//---------------------------------------------------------
inline
int  f_compare(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 4 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _symbol = _in[1] ;
    const std::string &  _a = _in[2] ;
    const std::string &  _b = _in[3] ;

    g_stack.push() ;

    int idx_a = t_code_to_value( _a ) ;
    if ( idx_a < 0 ) return idx_a ; // NG

    Value & a = g_stack.get_idx_value( idx_a ) ;

    a.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
    a.eval() ;          // 未評価の t_code や stream を評価して値にする

    if ( a.m_type != "int" )
        return RUNTIME_ERROR( "compare unmatch type." ) ; // "NG: compare : Value error" ; // 数字に変換できなかった 例外を投げる todo

    int idx_b = t_code_to_value( _b ) ;
    if ( idx_b < 0 ) return idx_b ; // NG

    Value & b = g_stack.get_idx_value( idx_b ) ;
    b.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
    b.eval() ;          // 未評価の t_code や stream を評価して値にする

    if ( b.m_type != "int" )
        return  RUNTIME_ERROR( "compare unmatch type." ) ; // "NG: compare : Value error" ; // 数字に変換できなかった 例外を投げる todo

    std::string  rtn ;
    if ( _symbol == "==" )
        rtn = ( a.m_num == b.m_num ? "-true" : "-false" ) ;
    else if ( _symbol == "!=" )
        rtn = ( a.m_num != b.m_num ? "-true" : "-false" ) ;
    else if ( _symbol == ">=" )
        rtn = ( a.m_num >= b.m_num ? "-true" : "-false" ) ;
    else if ( _symbol == "<=" )
        rtn = ( a.m_num <= b.m_num ? "-true" : "-false" ) ;
    else if ( _symbol == ">" )
        rtn = ( a.m_num > b.m_num ? "-true" : "-false" ) ;
    else if ( _symbol == "<" )
        rtn = ( a.m_num < b.m_num ? "-true" : "-false" ) ;
    else
        return PROGRAM_ERROR( "Not supported compare operators." ) ; // "NG: compare : program error"; // 知らない演算子

    g_stack.pop() ;

    int idx = t_code_to_value( rtn ) ;
    if ( idx < 0 ) return idx ; // NG

    Value & v = g_stack.get_idx_value( idx ) ;

    v.d += "[compare]" ; // デバッグ用なので、リリース時には消します。 todo

    return v.m_idx ; // "OK: compare";
}

//---------------------------------------------------------
inline
int  f_scope(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() < 1 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    if ( _in.size() == 1 )
    {
        // 空の scope は、空の list を返す
        Value & out = g_stack.alloc_value() ;

        out.d += "[scope]" ; // デバッグ用なので、リリース時には消します。 todo

        out.m_type = "list" ;
        out.m_str  = "[ ]" ;

        return out.m_idx ; // "OK: scope" ;
    }

    // 戻り値の Value を作る
    Value & last = g_stack.alloc_value() ;


    // define専用の boss Value を作る
    Value & boss = g_stack.alloc_value() ;

    boss.d += "[boss-scope]" ; // デバッグ用なので、リリース時には消します。 todo

    boss.m_type = "scope" ;
    boss.m_str  = "( )" ;

    // すべてを評価して、最後の値を返す
    g_stack.push_boss() ;
    g_stack.push() ;

    int idx = 1 ;
    for ( ; idx<_in.size()-1 ; idx++ )
    {{
        int idx_it = t_code_to_value( _in[idx] ) ;
        if ( idx_it < 0 ) return idx_it ; // NG

        Value & it = g_stack.get_idx_value( idx_it ) ;
        it.find_tag() ;     // unknown_type の場合は tag_search() して置き換える
            
        it.d += "[scope]" ; // デバッグ用なので、リリース時には消します。 todo
    }}

    // 最後の値を返す
    last.d += "[scope]" ; // デバッグ用なので、リリース時には消します。 todo

    last.m_type = "t_code" ;
    last.m_str = _in[idx] + " : to_be_evaluated" ;  // デバッグ用なのでリリース時には消します todo
    last.m_to_be_evaluated_list.push_back( _in[idx] ) ;

    last.eval() ;

    g_stack.pop() ; // すべて破棄

    g_stack.pop_boss() ;

    return last.m_idx ; // "OK: scope" ;
}

//---------------------------------------------------------
inline
int  f_if(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 4 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _symbol = _in[1] ;
    const std::string &  _a = _in[2] ;
    const std::string &  _b = _in[3] ;

    int idx_sym = t_code_to_value( _symbol ) ;
    if ( idx_sym < 0 ) return idx_sym ; // NG

    Value symbol = g_stack.get_idx_value( idx_sym ) ; // copy

    symbol.find_tag() ;
    symbol.eval() ;                  // 未評価の t_code や stream を評価して値にする

    const std::string & rtn = ( symbol.m_str == "-true" ? _a : _b ) ;

    int idx_v = t_code_to_value( rtn ) ;
    if ( idx_v < 0 ) return idx_v ; // NG

    Value & v = g_stack.get_idx_value( idx_v ) ;

    v.find_tag() ;
    v.eval() ;                  // 未評価の t_code や stream を評価して値にする

    v.d += "[if]" ; // デバッグ用なので、リリース時には消します。 todo

    return v.m_idx ; // "OK: if";
}

//---------------------------------------------------------
inline
int  f_operator(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 4 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _symbol = _in[1] ;
    const std::string &  _a = _in[2] ;
    const std::string &  _b = _in[3] ;

    g_stack.push() ;

    int idx_a = t_code_to_value( _a ) ;
    if ( idx_a < 0 ) return idx_a ; // NG

    Value & a = g_stack.get_idx_value( idx_a ) ;

    a.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
    a.eval() ;          // 未評価の t_code や stream を評価して値にする

    if ( a.m_type != "int" )
        if ( a.m_type != "stream" ) // 暫定的に stream もOKとする
            return RUNTIME_ERROR( "operator unmatch type. " << _a ) ; // "NG: operator : Value error" ; // 数字に変換できなかった 例外を投げる todo

    int idx_b = t_code_to_value( _b ) ;
    if ( idx_b < 0 ) return idx_b ; // NG

    Value & b = g_stack.get_idx_value( idx_b ) ;

    b.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
    b.eval() ;          // 未評価の t_code や stream を評価して値にする

    if ( b.m_type != "int" )
        if ( a.m_type != "stream" ) // 暫定的に stream もOKとする
            return RUNTIME_ERROR( "operator unmatch type. " << _b ) ; // "NG: operator : Value error" ; // 数字に変換できなかった 例外を投げる todo

    std::stringstream   no ;

    if      ( _symbol == "*"   ) no << ( a.m_num * b.m_num ) ;
    else if ( _symbol == "-/-" ) no << ( a.m_num / b.m_num ) ;
    else if ( _symbol == "%"   ) no << ( a.m_num % b.m_num ) ;
    else if ( _symbol == "+"   ) no << ( a.m_num + b.m_num ) ;
    else if ( _symbol == "-"   ) no << ( a.m_num - b.m_num ) ;
    else
        return PROGRAM_ERROR( "Not supported operators." ) ; // "NG: operator : program error"; // 知らない演算子

    g_stack.pop() ;

    int idx_v = t_code_to_value( no.str() ) ;
    if ( idx_v < 0 ) return idx_v ; // NG

    Value & v = g_stack.get_idx_value( idx_v ) ;

    v.find_tag() ;
    v.eval() ;                  // 未評価の t_code や stream を評価して値にする

    v.d += "[operator]" ; // デバッグ用なので、リリース時には消します。 todo

    return v.m_idx ; // "OK: operator" ;
}

//---------------------------------------------------------
// public-define
inline
int  f_define(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 3 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _a = _in[1] ;
    const std::string &  _f = _in[2] ;

    return g_stack.public_define( _a, _f ) ;
}

//---------------------------------------------------------
// private-define
inline
int  f_private(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 3 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _a = _in[1] ;
    const std::string &  _f = _in[2] ;

    return g_stack.private_define( _a, _f ) ;
}

//---------------------------------------------------------
inline
int  f_call(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 3 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _func = _in[1] ;
    const std::string &  _arg  = _in[2] ;

    g_stack.push_boss() ;

    int idx_arg = t_code_to_value( _arg ) ;
    if ( idx_arg < 0 ) return idx_arg ; // NG

    Value & arg = g_stack.get_idx_value( idx_arg ) ;

    arg.find_tag() ;        // unknown_type の場合は tag_search() して置き換える
    arg.eval() ;            // 未評価の t_code や stream を評価して値にする

    g_stack.public_define( "arg", arg.m_str ) ; // todo arg.m_str ではなく arg へのポインタを登録したい

    int idx_t = t_code_to_value( _func ) ;
    if ( idx_t < 0 ) return idx_t ; // NG

    Value & tmp = g_stack.get_idx_value( idx_t ) ;

    tmp.find_tag() ;        // unknown_type の場合は tag_search() して置き換える
    tmp.eval() ;            // 未評価の t_code や stream を評価して値にする

    if ( tmp.m_type == "list" )
    {
        tmp.m_type += "-instance" ;

        // listの中をすべて評価して・・・_out に入れる
        for ( auto & it : tmp.m_to_be_evaluated_list ) // 'list' の未評価値
        {{
            int idx_v = t_code_to_value( it ) ;
            if ( idx_v < 0 ) return idx_v ; // NG

            Value & v = g_stack.get_idx_value( idx_v ) ;

            v.find_tag() ;      // unknown_type の場合は tag_search() して置き換える

            v.d += "[" + _func + ":call]" ; // デバッグ用なので、リリース時には消します。 todo

            tmp.m_evaluated_index.push_back( v.m_idx ) ; // 実は誰も参照していない疑惑 todo
        }}
    }
    g_stack.pop_boss() ;

    tmp.d += "[" + _func + ":call]" ; // デバッグ用なので、リリース時には消します。 todo

    return tmp.m_idx ; // "OK: call" ;
}

//---------------------------------------------------------
inline
int  f_each(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 3 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _a = _in[1] ;
    const std::string &  _b = _in[2] ;

    int idx_a = t_code_to_value( _a ) ;
    if ( idx_a < 0 ) return idx_a ; // NG

    Value & a = g_stack.get_idx_value( idx_a ) ;

    a.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
    a.eval() ;          // 未評価の t_code や stream を評価して値にする

    // 以下数値だった場合 list や string だった場合はまだ未実装 todo

    if ( a.m_type == "int" )
    {
        Value & lst = g_stack.alloc_value() ; // list を作って返す

        lst.m_str = "[ " ;
        // list　の中をすべて評価して・・・_out に入れる
        g_stack.push_boss() ;

        for ( int i=0 ; i<a.m_num ; i++ )
        {{
            std::stringstream  no ;
            no << i ;

            Value & item = g_stack.alloc_value() ; // list の各項目の戻り値が入る

            g_stack.push_boss() ;

            g_stack.public_define( "it", no.str() ) ;

            int idx_b = t_code_to_value( _b ) ;
            if ( idx_b < 0 ) return idx_b ; // NG

            Value & b = g_stack.get_idx_value( idx_b ) ;

            b.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
            b.eval() ;          // 未評価の t_code や stream を評価して値にする

            lst.m_str += b.m_str + " " ; // todo 仮です（ホントは list を作る）

            lst.m_evaluated_index.push_back( b.m_idx ) ;

            g_stack.pop_boss() ;
        }}
        g_stack.pop_boss() ;

        lst.m_str += "]" ;
        lst.m_type = "list" ;

        lst.d += "[each]" ; // デバッグ用なので、リリース時には消します。 todo

        return lst.m_idx ; // OK
    }

    return PROGRAM_ERROR( "NG: Unsupported type." ) ;
}

//---------------------------------------------------------
inline
int  f_match(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 3 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _a = _in[1] ;
    const std::string &  _b = _in[2] ;

    if ( _a == "/" && _b == "co" )
    {
        // todo 特別暫定対応 ほんとは stream で作る

        Value & out = g_stack.alloc_value() ;

        out.d += "[" + _a + "." + _b + ":match]" ; // デバッグ用なので、リリース時には消します。 todo
        out.m_type = "todo" ;
        out.m_str = "OK: match /.co" ;

        return out.m_idx ; // "OK: match" ;
    }

    if ( _b == "type" )
    {
        Value & out = g_stack.alloc_value() ;

        out.d += "[" + _a + "." + _b + ":match]" ; // デバッグ用なので、リリース時には消します。 todo
        g_stack.push() ;
        {
            int idx_a = t_code_to_value( _a ) ;
            if ( idx_a < 0 ) return idx_a ; // NG

            Value & a = g_stack.get_idx_value( idx_a ) ;

            a.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
            a.eval() ;          // 未評価の t_code や stream を評価して値にする

            out.m_str = a.m_type ;
            out.m_type = "str" ;
        }
        g_stack.pop() ;

        return  out.m_idx ; // "OK: match" ;
    }

    if ( _a == "arg" && _b == "0" ) // 暫定対応 todo ホントはargはlist型になる予定ここでは値が１しか来てない
    {
        int idx_a = t_code_to_value( _a ) ;
        if ( idx_a < 0 ) return idx_a ; // NG

        Value & a = g_stack.get_idx_value( idx_a ) ;

        a.find_tag() ;  // unknown_type の場合は tag_search() して置き換える

        a.d += "[" + _a + "." + _b + ":match]" ; // デバッグ用なので、リリース時には消します。 todo

        return  a.m_idx ; // "OK: match" ;
    }

    if ( _b == "0" ) // 暫定対応 todo ホントは数字だったら
    {
        int idx_a = t_code_to_value( _a ) ;
        if ( idx_a < 0 ) return idx_a ; // NG

        Value & a = g_stack.get_idx_value( idx_a ) ;

        a.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
        a.eval() ;          // 未評価の t_code や stream を評価して値にする

        if ( a.m_type == "int" )
        {
            a.m_str = a.m_str + ".0" ; // 暫定対応 todo
            a.m_type = "float" ;

            a.d += "[" + _a + "." + _b + ":match]" ; // デバッグ用なので、リリース時には消します。 todo

            return  a.m_idx ; // "OK: match" ;
        }
    }
    else if ( _b == "float" )
    {
        g_stack.push_boss() ;
        g_stack.push() ;

        int idx_a = t_code_to_value( _a ) ;
        if ( idx_a < 0 ) return idx_a ; // NG

        Value & a = g_stack.get_idx_value( idx_a ) ;

        a.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
        a.eval() ;          // 未評価の t_code や stream を評価して値にする

        if ( a.m_type == "int" )
        {
            a.m_str = a.m_str + ".0" ;
            a.m_type = "float" ;
            a.d += "[" + _a + "." + _b + ":match]" ; // デバッグ用なので、リリース時には消します。 todo

            g_stack.pop() ;
            g_stack.pop_boss() ;

            return  a.m_idx ; // "OK: match" ;
        }
        g_stack.pop() ;
        g_stack.pop_boss() ;
    }

    int idx_a = t_code_to_value( _a ) ;
    if ( idx_a < 0 ) return idx_a ; // NG

    if ( g_stack.get_idx_value( idx_a ).m_type == "unknown_type" )
    idx_a = find_tag( _a ) ;
    if ( idx_a < 0 ) return idx_a ; // NG

    Value & a = g_stack.get_idx_value( idx_a ) ;
    a.eval() ;        // 未評価の t_code や stream を評価して値にする

    if ( a.m_type == "list-instance" )
    {
        // list 内の define _b を探して返す

        g_stack.m_boss_cd.push_back( a.m_boss ) ;
        std::cout << "---->" << a.m_boss << "\n" ; // 積む

        int idx = a.tag_search( _b ) ;

        if ( idx < 0 )
        {
            std::cout << "<----\n" ; 
            g_stack.m_boss_cd.pop_back() ;
            return false ; // undefine
        }

        Value & v = g_stack.get_idx_value( idx ) ;

        v.d += "[" + _a + "." + _b + ":match]" ; // デバッグ用なので、リリース時には消します。 todo

//        std::cout << "<----\n" ; // けどもどさなくていいらしい（むしろ戻すと見つけられなくなる）
  //      g_stack.m_boss_cd.pop_back() ;

        return idx ; // OK
    }

    // out が list 以外だったら？？？？どうなるの？？

    return PROGRAM_ERROR( "NG: match" ) ;
}


//---------------------------------------------------------
inline
int  f_push(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() != 3 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    const std::string &  _a = _in[1] ;
    const std::string &  _b = _in[2] ;

    int idx_a = t_code_to_value( _a ) ;
    if ( idx_a < 0 ) return idx_a ; // NG

    Value & tmp = g_stack.get_idx_value( idx_a ) ;

    if ( tmp.m_type == "unknown_type" )
        idx_a = find_tag( tmp.m_str ) ;

    if ( idx_a < 0 )
        return -1 ; // not found.

    Value & a = g_stack.get_idx_value( idx_a ) ;
    a.eval() ;                  // 未評価の t_code や stream を評価して値にする

    int idx_b = t_code_to_value( _b ) ;
    if ( idx_b < 0 ) return idx_b ; // NG

    Value & b = g_stack.get_idx_value( idx_b ) ;

    b.find_tag() ;      // unknown_type の場合は tag_search() して置き換える
    b.eval() ;          // 未評価の t_code や stream を評価して値にする

    if ( a.m_str == "OK: match /.co" ) // todo 暫定対応　ホントは stream じゃないと push できない
    {
        if ( b.m_type == "list" )
        {
            puts( "[ " ) ;

            // デバッグ用のバッファ
            g_co_str += "[ " ;

            for ( auto & it : b.m_evaluated_index )
            {{
                Value & v = g_stack.get_idx_value( it ) ;

                v.eval() ;  // 未評価の t_code や stream を評価して値にする

                puts( v.m_str.c_str() ) ;

                // デバッグ用のバッファ
                g_co_str += v.m_str + " " ;
            }}
            puts( "]" ) ;

            // デバッグ用のバッファ
            g_co_str += "]" ;
        }
        else
        {
            puts( b.m_str.c_str() ) ;

            // デバッグ用のバッファ
            g_co_str += b.m_str ;
        }

        return a.m_idx ; // "OK: push" ;
    }

    if ( a.m_type == "stream" )
    {
        a.m_str = b.m_str ; // todo 簡易版（注意！安易に代入している！！

        return a.m_idx ; // "OK: push" ;
    }

    return a.m_idx ; // "OK: push" ;
}

//---------------------------------------------------------
inline
int  f_list(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() <= 1 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    Value & out = g_stack.alloc_value() ;

    out.d += "[list]" ; // デバッグ用なので、リリース時には消します。 todo

    out.m_type = "list" ;
    out.m_str  = "[ " ; // dummy todo はじめは空で、call されて評価される

    // to_be_evaluated_listの登録、ここでdefineはせずに、call時にargといっしょにdefineする
    for ( auto & it : _in )
    {{
        if ( it == "list" ) continue ;

        out.m_to_be_evaluated_list.push_back( it ) ;
    }}

    return out.m_idx ; // "OK: list" ;
}

//---------------------------------------------------------
inline
int  f_stream(
    const std::vector< std::string > &  _in
)
{
    if ( _in.size() <= 1 )
        return PROGRAM_ERROR( "param size mismatch." ) ;

    Value & out = g_stack.alloc_value() ;

    out.d += "[stream]" ; // デバッグ用なので、リリース時には消します。 todo

    out.m_type = "stream" ;
    out.m_str = "{ "; // dummy todo

    // to_be_evaluated_list
    for ( auto & it : _in )
    {{
        if ( it == "stream" ) continue ;

        out.m_to_be_evaluated_list.push_back( it ) ;
    }}

    return out.m_idx ; // "OK: stream" ;
}


//---------------------------------------------------------
int run(
    std::vector< std::string > & _t
)
{
    if ( _t[0] == "scope"    ) return f_scope( _t ) ;
    if ( _t[0] == "match"    ) return f_match( _t ) ;
    if ( _t[0] == "define"   ) return f_define( _t ) ;
    if ( _t[0] == "public"   ) return f_define( _t ) ; // 暫定対応 todo ホントは private_define と public_define は分ける
    if ( _t[0] == "compare"  ) return f_compare( _t ) ;
    if ( _t[0] == "operator" ) return f_operator( _t ) ;
    if ( _t[0] == "call"     ) return f_call( _t ) ;
    if ( _t[0] == "each"     ) return f_each( _t ) ;
    if ( _t[0] == "if"       ) return f_if( _t ) ;
    if ( _t[0] == "push"     ) return f_push( _t ) ;
    if ( _t[0] == "list"     ) return f_list( _t ) ;
    if ( _t[0] == "stream"   ) return f_stream( _t ) ;

    return PROGRAM_ERROR( "unsupported command." ) ; // 未サポートなコマンド
}
