// Copyright (c) 2021 SmartPolarBear
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by cleve on 11/9/2021.
//

#pragma once

#include <vector>

namespace clox::resolving
{

enum ast_annotation_type
{

};

class ast_annotation
{
public:
	virtual ast_annotation_type type() = 0;
};

class annotatable_ast_node_base
{
public:
	void annotate(const std::shared_ptr<ast_annotation>& annotation)
	{
		ast_annotations_.insert_or_assign(annotation->type(), annotation);
	}

	bool contains_annotation(ast_annotation_type t)
	{
		return ast_annotations_.contains(t);
	}

	std::shared_ptr<ast_annotation> get_annotation(ast_annotation_type t)
	{
		return ast_annotations_.at(t);
	}

protected:
	std::unordered_map<ast_annotation_type, std::shared_ptr<ast_annotation>> ast_annotations_{};
};

}