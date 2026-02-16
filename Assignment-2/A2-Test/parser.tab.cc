// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.tab.hh"


// Unqualified %code blocks.
#line 16 "parser.y"

    #include "lexer.hpp"
    parser::Parser::symbol_type yylex(Lexer&);

#line 51 "parser.tab.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 4 "parser.y"
namespace parser {
#line 144 "parser.tab.cc"

  /// Build a parser object.
  Parser::Parser (Lexer& lexer_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      lexer (lexer_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_FLOAT_NUM: // FLOAT_NUM
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INT_NUM: // INT_NUM
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STR_CONST: // STR_CONST
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_FLOAT_NUM: // FLOAT_NUM
        value.move< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INT_NUM: // INT_NUM
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STR_CONST: // STR_CONST
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_FLOAT_NUM: // FLOAT_NUM
        value.copy< float > (that.value);
        break;

      case symbol_kind::S_INT_NUM: // INT_NUM
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STR_CONST: // STR_CONST
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_FLOAT_NUM: // FLOAT_NUM
        value.move< float > (that.value);
        break;

      case symbol_kind::S_INT_NUM: // INT_NUM
        value.move< int > (that.value);
        break;

      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STR_CONST: // STR_CONST
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (lexer));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_FLOAT_NUM: // FLOAT_NUM
        yylhs.value.emplace< float > ();
        break;

      case symbol_kind::S_INT_NUM: // INT_NUM
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_STR_CONST: // STR_CONST
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 45: // expression: MINUS expression
#line 171 "parser.y"
                       {}
#line 603 "parser.tab.cc"
    break;


#line 607 "parser.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0









  const signed char Parser::yypact_ninf_ = -67;

  const signed char Parser::yytable_ninf_ = -12;

  const short
  Parser::yypact_[] =
  {
     179,   -67,   -67,   -67,   -67,   -67,     9,   179,   179,    -8,
     -67,   -67,    58,   -67,   179,   -67,   -67,   179,    56,   -67,
     124,    65,    11,   179,    60,   -67,   165,    40,   -67,   -67,
     -67,   -67,   -21,   -67,   101,   -67,   102,   -67,   -67,    83,
      67,   -67,   179,   -19,    46,   -67,   -67,    97,   -67,   179,
     -67,   -67,   -67,   179,     5,   -67,   -67,   140,   -67,   142,
     131,   -67,   -67,   -67,   -67,     8,   142,   -67,   -67,   -67,
     -67,   142,   142,   142,    69,   -67,   -67,   130,   -67,    86,
      64,   -67,   103,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   -67,   142,   -67,   -67,   -67,
      64,    64,   -67,   -67,   184,   184,   117,   117,   193,   193,
     172,   160,    41,   142,   125
  };

  const signed char
  Parser::yydefact_[] =
  {
       0,    33,    37,    35,    36,    34,     0,     0,     0,     0,
       2,     8,     0,     1,     0,     4,     7,     0,     0,     3,
       0,    32,     0,     0,     0,     5,     0,     0,    17,    19,
      20,    18,     0,    15,     0,    30,     0,     6,    32,     0,
       0,    10,    26,     0,     0,    16,    31,     0,    22,    27,
      29,    14,     9,    26,     0,    28,    22,     0,    13,     0,
       0,    21,    23,    24,    25,     0,     0,    60,    61,    47,
      62,     0,     0,     0,     0,    53,    48,     0,    12,     0,
      45,    52,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,     0,    40,    38,    46,
      41,    42,    43,    44,    56,    54,    57,    55,    58,    59,
      50,    51,     0,     0,    49
  };

  const short
  Parser::yypgoto_[] =
  {
     -67,   -67,    53,   149,     1,    29,   133,   118,   -67,   104,
     -67,   110,   -67,    22,   -67,   -11,   -67,   -67,   -67,   -66,
     -67,   -67
  };

  const signed char
  Parser::yydefgoto_[] =
  {
       0,     6,     7,     8,    18,    10,    32,    33,    34,    54,
      61,    48,    49,    11,    22,    12,    62,    63,    64,    74,
      75,    76
  };

  const signed char
  Parser::yytable_[] =
  {
      79,     9,    43,    24,    44,    80,    81,    82,     9,    13,
      57,    28,    29,    57,    30,    31,    20,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,    16,
     112,    24,    58,    35,    36,    78,    15,    19,    24,    16,
      59,    60,    24,    59,    60,    16,    25,   114,    83,    84,
      85,    86,    37,    87,    88,    89,    90,    91,    92,    93,
      94,    17,    41,    21,    50,    38,    42,    23,    52,    96,
     113,    55,    53,    85,    86,    50,    83,    84,    85,    86,
      26,    87,    88,    89,    90,    91,    92,    93,    94,   -11,
      43,    95,    47,    83,    84,    85,    86,    96,    87,    88,
      89,    90,    91,    92,    93,    94,    45,    46,    98,    42,
      83,    84,    85,    86,    96,    87,    88,    89,    90,    91,
      92,    93,    94,    53,    83,    84,    85,    86,    99,    87,
      88,    96,    83,    84,    85,    86,    77,    87,    88,    89,
      90,    91,    92,    93,    94,    67,    68,    69,    70,    27,
      71,    66,    97,   -12,    28,    29,    14,    30,    31,    40,
      65,    51,    72,    56,     0,     0,    73,    83,    84,    85,
      86,     0,    87,    88,    89,    90,    91,    92,    93,    83,
      84,    85,    86,     0,    87,    88,    89,    90,    91,    92,
      39,    83,    84,    85,    86,    28,    29,     0,    30,    31,
      83,    84,    85,    86,     0,    87,    88,    89,    90,     1,
       2,     3,     4,     5
  };

  const signed char
  Parser::yycheck_[] =
  {
      66,     0,    23,    14,    25,    71,    72,    73,     7,     0,
       5,    30,    31,     5,    33,    34,    24,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,     7,
      96,    42,    27,    22,    23,    27,     7,     8,    49,    17,
      35,    36,    53,    35,    36,    23,    17,   113,     7,     8,
       9,    10,    23,    12,    13,    14,    15,    16,    17,    18,
      19,     8,    22,     5,    42,     5,    26,    14,    22,    28,
      29,    49,    26,     9,    10,    53,     7,     8,     9,    10,
      24,    12,    13,    14,    15,    16,    17,    18,    19,    24,
      23,    22,    25,     7,     8,     9,    10,    28,    12,    13,
      14,    15,    16,    17,    18,    19,     5,     5,    22,    26,
       7,     8,     9,    10,    28,    12,    13,    14,    15,    16,
      17,    18,    19,    26,     7,     8,     9,    10,    25,    12,
      13,    28,     7,     8,     9,    10,     5,    12,    13,    14,
      15,    16,    17,    18,    19,     3,     4,     5,     6,    25,
       8,    11,    22,    28,    30,    31,     7,    33,    34,    26,
      56,    43,    20,    53,    -1,    -1,    24,     7,     8,     9,
      10,    -1,    12,    13,    14,    15,    16,    17,    18,     7,
       8,     9,    10,    -1,    12,    13,    14,    15,    16,    17,
      25,     7,     8,     9,    10,    30,    31,    -1,    33,    34,
       7,     8,     9,    10,    -1,    12,    13,    14,    15,    30,
      31,    32,    33,    34
  };

  const signed char
  Parser::yystos_[] =
  {
       0,    30,    31,    32,    33,    34,    40,    41,    42,    43,
      44,    52,    54,     0,    42,    44,    52,    41,    43,    44,
      24,     5,    53,    41,    54,    44,    24,    25,    30,    31,
      33,    34,    45,    46,    47,    22,    23,    44,     5,    25,
      45,    22,    26,    23,    25,     5,     5,    25,    50,    51,
      52,    46,    22,    26,    48,    52,    50,     5,    27,    35,
      36,    49,    55,    56,    57,    48,    11,     3,     4,     5,
       6,     8,    20,    24,    58,    59,    60,     5,    27,    58,
      58,    58,    58,     7,     8,     9,    10,    12,    13,    14,
      15,    16,    17,    18,    19,    22,    28,    22,    22,    25,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    29,    58
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    39,    40,    40,    40,    40,    40,    41,    41,    42,
      42,    43,    44,    44,    45,    45,    46,    47,    47,    47,
      47,    48,    48,    49,    49,    49,    50,    50,    51,    51,
      52,    53,    53,    54,    54,    54,    54,    54,    55,    56,
      57,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    59,    59,    59,    59,    59,    59,
      60,    60,    60
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     2,     2,     3,     4,     2,     1,     5,
       4,     2,     8,     7,     3,     1,     2,     1,     1,     1,
       1,     2,     0,     1,     1,     1,     0,     1,     2,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     4,     3,
       3,     3,     3,     3,     3,     2,     3,     1,     1,     5,
       3,     3,     2,     1,     3,     3,     3,     3,     3,     3,
       1,     1,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "INT_NUM", "FLOAT_NUM",
  "NAME", "STR_CONST", "PLUS", "MINUS", "MULT", "DIV", "ASSIGN_OP",
  "GREATER_THAN", "LESS_THAN", "GREATER_THAN_EQUAL", "LESS_THAN_EQUAL",
  "EQUAL", "NOT_EQUAL", "AND", "OR", "NOT", "ADDRESSOF", "SEMICOLON",
  "COMMA", "LEFT_ROUND_BRACKET", "RIGHT_ROUND_BRACKET",
  "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "QUESTION_MARK", "COLON",
  "INTEGER", "BOOL", "VOID", "STRING", "FLOAT", "WRITE", "READ", "RET",
  "UMINUS", "$accept", "program", "var_list", "func_decl", "func_header",
  "func_def", "formal_param_list", "formal_param", "param_type",
  "statement_list", "statement", "optional_var_decl_stmt_list",
  "var_decl_stmt_list", "var_decl_stmt", "var_decl_item_list",
  "named_type", "assignment_statement", "print_statement",
  "read_statement", "expression", "rel_expression", "constant_as_operand", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,    57,    57,    58,    59,    60,    61,    74,    75,    79,
      80,    91,    95,    96,   100,   101,   105,   109,   110,   111,
     112,   116,   117,   121,   122,   123,   127,   128,   133,   134,
     138,   142,   143,   147,   148,   149,   150,   151,   155,   159,
     163,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   183,   184,   185,   186,   187,   188,
     192,   193,   194
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 4 "parser.y"
} // parser
#line 1016 "parser.tab.cc"

#line 197 "parser.y"

