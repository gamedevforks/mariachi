// Hive Mariachi Engine
// Copyright (C) 2008 Hive Solutions Lda.
//
// This file is part of Hive Mariachi Engine.
//
// Hive Mariachi Engine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hive Mariachi Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hive Mariachi Engine. If not, see <http://www.gnu.org/licenses/>.

// __author__    = Jo�o Magalh�es <joamag@hive.pt>
// __version__   = 1.0.0
// __revision__  = $LastChangedRevision$
// __date__      = $LastChangedDate$
// __copyright__ = Copyright (c) 2008 Hive Solutions Lda.
// __license__   = GNU General Public License (GPL), Version 3

#pragma once

# include "features.h"
# include "value.h"
# include <deque>
# include <stack>
# include <string>
# include <iostream>

namespace Json {

   /**
    * \brief Unserialize a <a HREF="http://www.json.org">JSON</a> document into a Value.
    */
   class Reader {
   public:
      typedef char Char;
      typedef const Char *Location;

      /** \brief Constructs a Reader allowing all features
       * for parsing.
       */
      Reader();

      /** \brief Constructs a Reader allowing the specified feature set
       * for parsing.
       */
      Reader( const Features &features );

      /** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a> document.
       * \param document utf-8 encoded string containing the document to read.
       * \param root [out] Contains the root value of the document if it was
       *             successfully parsed.
       * \param collectComments \c true to collect comment and allow writing them back during
       *                        serialization, \c false to discard comments.
       *                        This parameter is ignored if Features::allowComments_
       *                        is \c false.
       * \return \c true if the document was successfully parsed, \c false if an error occurred.
       */
      bool parse( const std::string &document,
                  Value &root,
                  bool collectComments = true );

      /** \brief Read a Value from a <a HREF="http://www.json.org">JSON</a> document.
       * \param document utf-8 encoded string containing the document to read.
       * \param root [out] Contains the root value of the document if it was
       *             successfully parsed.
       * \param collectComments \c true to collect comment and allow writing them back during
       *                        serialization, \c false to discard comments.
       *                        This parameter is ignored if Features::allowComments_
       *                        is \c false.
       * \return \c true if the document was successfully parsed, \c false if an error occurred.
       */
      bool parse( const char *beginDoc, const char *endDoc,
                  Value &root,
                  bool collectComments = true );

      /// \brief Parse from input stream.
      /// \see Json::operator>>(std::istream&, Json::Value&).
      bool parse( std::istream &is,
                  Value &root,
                  bool collectComments = true );

      /** \brief Returns a user friendly string that list errors in the parsed document.
       * \return Formatted error message with the list of errors with their location in
       *         the parsed document. An empty string is returned if no error occurred
       *         during parsing.
       */
      std::string getFormatedErrorMessages() const;

   private:
      enum TokenType
      {
         tokenEndOfStream = 0,
         tokenObjectBegin,
         tokenObjectEnd,
         tokenArrayBegin,
         tokenArrayEnd,
         tokenString,
         tokenNumber,
         tokenTrue,
         tokenFalse,
         tokenNull,
         tokenArraySeparator,
         tokenMemberSeparator,
         tokenComment,
         tokenError
      };

      class Token
      {
      public:
         TokenType type_;
         Location start_;
         Location end_;
      };

      class ErrorInfo
      {
      public:
         Token token_;
         std::string message_;
         Location extra_;
      };

      typedef std::deque<ErrorInfo> Errors;

      bool expectToken( TokenType type, Token &token, const char *message );
      bool readToken( Token &token );
      void skipSpaces();
      bool match( Location pattern,
                  int patternLength );
      bool readComment();
      bool readCStyleComment();
      bool readCppStyleComment();
      bool readString();
      void readNumber();
      bool readValue();
      bool readObject( Token &token );
      bool readArray( Token &token );
      bool decodeNumber( Token &token );
      bool decodeString( Token &token );
      bool decodeString( Token &token, std::string &decoded );
      bool decodeDouble( Token &token );
      bool decodeUnicodeCodePoint( Token &token,
                                   Location &current,
                                   Location end,
                                   unsigned int &unicode );
      bool decodeUnicodeEscapeSequence( Token &token,
                                        Location &current,
                                        Location end,
                                        unsigned int &unicode );
      bool addError( const std::string &message,
                     Token &token,
                     Location extra = 0 );
      bool recoverFromError( TokenType skipUntilToken );
      bool addErrorAndRecover( const std::string &message,
                               Token &token,
                               TokenType skipUntilToken );
      void skipUntilSpace();
      Value &currentValue();
      Char getNextChar();
      void getLocationLineAndColumn( Location location,
                                     int &line,
                                     int &column ) const;
      std::string getLocationLineAndColumn( Location location ) const;
      void addComment( Location begin,
                       Location end,
                       CommentPlacement placement );
      void skipCommentTokens( Token &token );

      typedef std::stack<Value *> Nodes;
      Nodes nodes_;
      Errors errors_;
      std::string document_;
      Location begin_;
      Location end_;
      Location current_;
      Location lastValueEnd_;
      Value *lastValue_;
      std::string commentsBefore_;
      Features features_;
      bool collectComments_;
   };

   /** \brief Read from 'sin' into 'root'.

    Always keep comments from the input JSON.

    This can be used to read a file into a particular sub-object.
    For example:
    \code
    Json::Value root;
    cin >> root["dir"]["file"];
    cout << root;
    \endcode
    Result:
    \verbatim
    {
    "dir": {
        "file": {
        // The input stream JSON would be nested here.
        }
    }
    }
    \endverbatim
    \throw std::exception on parse error.
    \see Json::operator<<()
   */
   std::istream& operator>>( std::istream&, Value& );

}
