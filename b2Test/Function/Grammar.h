//
//  Grammar.h
//  b2Test
//
//  Created by Collins, James B. on 5/4/15.
//  Copyright (c) 2015 West Texas A&M University. All rights reserved.
//

#ifndef b2Test_Grammar_h
#define b2Test_Grammar_h

#define BOOST_RESULT_OF_USE_DECLTYPE
#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/bind.hpp>



#include "Node.h"
#include "SumOperator.h"
#include "MultOperator.h"
#include "NegateOperator.h"
#include "ExpOperator.h"
#include "Constant.h"
#include "Variable.h"




namespace parser
{
    
    
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    
    
    
    struct var_ : qi::symbols<char,int>
    {
        
    }var;
    
    
    
    int varcount = 0;
    void addVars(std::vector<char> const& c)
    {
        std::string str;
        for(auto it : c)
        {
            str = str + it;
        }
        var.add(str,varcount);
        varcount++;
    }
    
    template<typename Iterator>
    struct variables : qi::grammar<Iterator,ascii::space_type>
    {
        
        variables() : variables::base_type(start)
        {
            namespace phx = boost::phoenix;
            using qi::_1;
            
            start = (*qi::alnum)[&addVars] % ",";
        }
        
        qi::rule<Iterator,ascii::space_type> start;
    };

    template<typename Iterator>
    struct polynomial : qi::grammar<Iterator,Node*(),ascii::space_type>
    {
        
        polynomial() : polynomial::base_type(sumexpr)
        {
            namespace phx = boost::phoenix;
            using qi::_1;
            using qi::_val;
            using qi::eps;
            
            
            /////////// SUM TERMS ////////////////
            sumexpr = (qi::eps)[_val = phx::new_<SumOperator>()] >> //Still need possible negate operator!!!!!
            ( multexpr[ phx::bind( [](Node* input, Node* addinput)
                           {
                               dynamic_cast<SumOperator*>(input)->add_Child(std::shared_ptr<Node>(addinput), true);
                           }
                           ,_val, _1)] |
             ( '-'>> multexpr)[ phx::bind( [](Node* input, Node* addinput)
                                          {
                                              std::shared_ptr<NegateOperator> tempNeg = std::make_shared<NegateOperator>();
                                              tempNeg->add_Child(std::shared_ptr<Node>(addinput));
                                              dynamic_cast<SumOperator*>(input)->add_Child(std::shared_ptr<Node>(tempNeg));
                                          }
                                          ,_val, _1)]) >>
            *( ('-' >> multexpr[ phx::bind( [](Node* input, Node* addinput)
                                           {
                                               dynamic_cast<SumOperator*>(input)->add_Child(std::shared_ptr<Node>(addinput), false);
                                           }
                                           ,_val, _1)] ) |
              ('+' >> multexpr[ phx::bind( [](Node* input, Node* addinput)
                                          {
                                              dynamic_cast<SumOperator*>(input)->add_Child(std::shared_ptr<Node>(addinput), true);
                                          }
                                          ,_val, _1)] ) ) >>
            //Erase SumOp if there is only one term(i.e. no real summation happening)
            eps[ phx::bind( [](Node* &input)
                           {
                               int num = dynamic_cast<NaryOperator*>(input)->num_Children();
                               if(num == 1)
                               {
                                   Node* temp = dynamic_cast<NaryOperator*>(input)->get_first_child().get();
                                   delete input;  //This is dangerous!!!!!!
                                   input = temp;
                               }
                           },_val)];

            
            
            /////////// MULT FACTORS ////////////////
            multexpr = eps[_val = phx::new_<MultOperator>()] >>
            subexpr[ phx::bind( [](Node* input, Node* multinput)
                               {
                                   input->add_Child(std::shared_ptr<Node>(multinput));
                               }
                               ,_val, _1)] >>
            *( '*' >> subexpr )[ phx::bind( [](Node* input, Node* multinput)
                                           {
                                               input->add_Child(std::shared_ptr<Node>(multinput));
                                           }
                                           ,_val, _1)] >>
            //Erase MultOp if there is only one factor(i.e. no real multiplication happening)
            eps[ phx::bind( [](Node* &input)
                             {
                                 int num = dynamic_cast<NaryOperator*>(input)->num_Children();
                                 if(num == 1)
                                 {
                                     Node* temp = dynamic_cast<NaryOperator*>(input)->get_first_child().get();
                                     delete input;  //This is dangerous!!!!!!
                                     input = temp;
                                 }
                             },_val)];

            
            /////////// BASE OR PARENS ////////////////
            subexpr = base[_val = _1] | parenexpr[_val = _1];
            
            
            /////////// PARENS WITH EXP ////////////////
            parenexpr = eps[_val = phx::new_<ExpOperator>()] >>
            '(' >> sumexpr[ phx::bind( [](Node* input, Node* addinput)
                                      {
                                          input->add_Child(std::shared_ptr<Node>(addinput));
                                      }
                                      ,_val, _1)] >> ')' >>
            -( '^' >> qi::int_[ phx::bind( [](Node* input, int expinput)
                                          {
                                              dynamic_cast<ExpOperator*>(input)->setExponent(expinput);
                                          }
                                          ,_val, _1)] ) >>
            //Erase ExpOp if exp = 1(i.e. no real exponentiation happening)
            eps[ phx::bind( [](Node* &input)
                           {
                               int exp = dynamic_cast<ExpOperator*>(input)->getExponent();
                               if(exp == 1)
                               {
                                   Node* temp = dynamic_cast<UnaryOperator*>(input)->get_first_child().get();
                                   delete input;  //This is dangerous!!!!!!
                                   input = temp;
                               }
                           },_val)];
            
            
            /////////// NUMBER OR VARIABLE WITH EXP ////////////////
            base = ( qi::double_[_val = phx::new_<Constant>(_1)] ) |
                        (
                         eps[_val = phx::new_<ExpOperator>()] >>
                         var[ phx::bind( [](Node* input, int varnum)
                                        {
                                            input->add_Child(std::make_shared<Variable>(varnum));
                                        }
                                        ,_val, _1)] >>
                         -('^' >> qi::int_[ phx::bind( [](Node* input, int expinput)
                                                      {
                                                          dynamic_cast<ExpOperator*>(input)->setExponent(expinput);
                                                      }
                                                      ,_val, _1)]) >>
                         //Erase ExpOp if exp = 1(i.e. no real exponentiation happening)
                         eps[ phx::bind( [](Node* &input)
                                        {
                                            int exp = dynamic_cast<ExpOperator*>(input)->getExponent();
                                            if(exp == 1)
                                            {
                                                Node* temp = dynamic_cast<UnaryOperator*>(input)->get_first_child().get();
                                                delete input;  //This is dangerous!!!!!!
                                                input = temp;
                                            }
                                        },_val)] );

            
            
            
//            sumexpr = eps[_val = phx::new_<Node>("sum")] >>
//            eps[phx::bind(&Node::addLeaf,_val,new Node("null"))] >>
//            multexpr[phx::bind(&Node::addLeaf,_val,_1)] >>
//            *( ('-' >> multexpr[phx::bind(&Node::addLeaf,_val,_1)]) |
//              ('+' >> multexpr[phx::bind(&Node::addLeaf,_val,_1)]) );
//            
//            multexpr = eps[_val = phx::new_<Node>("mult")] >>
//            eps[phx::bind(&Node::addLeaf,_val,new Node("null"))] >>
//            subexpr[phx::bind(&Node::addLeaf,_val,_1)] >>
//            *( '*' >> subexpr )[phx::bind(&Node::addLeaf,_val,_1)];
//            
//            subexpr = base[_val = _1] | parenexpr[_val = _1];
//            
//            parenexpr = eps[_val = phx::new_<Node>("exp^1")] >>
//            '(' >> sumexpr[phx::bind(&Node::addLeaf,_val,new Node("null")), phx::bind(&Node::addLeaf,_val,_1)] >> ')' >>
//            -( '^' >> qi::int_[phx::bind(&Node::setop,_val,_1)] );
//            
//            
//            base = ( qi::double_[_val = phx::new_<Node>(_1)] ) |
//            ( eps[_val = phx::new_<Node>("exp^1")] >>
//             var[phx::bind(&Node::addLeaf,_val,new Node("null")), phx::bind(&Node::addLeaf,_val,phx::new_<Node>(_1))] >>
//             -('^' >> qi::int_[phx::bind(&Node::setop,_val,_1)]) );
            
        }
        
        qi::rule<Iterator,Node*(),ascii::space_type> sumexpr;
        qi::rule<Iterator,Node*(),ascii::space_type> multexpr;
        qi::rule<Iterator,Node*(),ascii::space_type> subexpr;
        qi::rule<Iterator,Node*(),ascii::space_type> parenexpr;
        qi::rule<Iterator,Node*(),ascii::space_type> base;
    };
}
#endif
