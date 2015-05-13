/*
 * Copyright (C) 2015 John Eric Martin <john.eric.martin@gmail.com>
 *
 * This file is part of State of Flux.
 *
 * State of Flux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * State of Flux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with State of Flux.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "VerilogHighlighter.h"

VerilogHighlighter::VerilogHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent), mIdentPattern("\\.([a-zA-Z_][a-zA-Z0-9_$]*|\\\\[^ \t\f\r]+)"),
    mNumberPattern("\\b([0-9]+\\'[bhd]){0,1}[_zZxX0-9]+\\b"), mSingleCommentPattern("\\/\\/.*"),
    mStartCommentPattern("/\\*"), mEndCommentPattern("\\*/")
{
    //mKeywordFormat.setFontWeight(QFont::Bold);
    mKeywordFormat.setForeground(QColor("#F92672"));
    mIdentFormat.setForeground(QColor("#66D9EF"));
    mSignalFormat.setForeground(QColor("#66D9EF"));
    mNumberFormat.setForeground(QColor("#AE81FF"));
    mCommentFormat.setForeground(QColor("#75715E"));

    QStringList keywords(QStringList()
        << QString("accept_on")
        << QString("alias")
        << QString("always")
        << QString("always_comb")
        << QString("always_ff")
        << QString("always_latch")
        << QString("and")
        << QString("assert")
        << QString("assign")
        << QString("assume")
        << QString("automatic")
        << QString("before")
        << QString("begin")
        << QString("bind")
        << QString("bins")
        << QString("binsof")
        << QString("bit")
        << QString("break")
        << QString("buf")
        << QString("bufif0")
        << QString("bufif1")
        << QString("byte")
        << QString("case")
        << QString("casex")
        << QString("casez")
        << QString("cell")
        << QString("chandle")
        << QString("checker")
        << QString("class")
        << QString("clocking")
        << QString("cmos")
        << QString("config")
        << QString("const")
        << QString("constraint")
        << QString("context")
        << QString("continue")
        << QString("cover")
        << QString("covergroup")
        << QString("coverpoint")
        << QString("cross")
        << QString("deassign")
        << QString("default")
        << QString("defparam")
        << QString("design")
        << QString("disable")
        << QString("dist")
        << QString("do")
        << QString("edge")
        << QString("else")
        << QString("end")
        << QString("endcase")
        << QString("endchecker")
        << QString("endclass")
        << QString("endclocking")
        << QString("endconfig")
        << QString("endfunction")
        << QString("endgenerate")
        << QString("endgroup")
        << QString("endinterface")
        << QString("endmodule")
        << QString("endpackage")
        << QString("endprimitive")
        << QString("endprogram")
        << QString("endproperty")
        << QString("endspecify")
        << QString("endsequence")
        << QString("endtable")
        << QString("endtask")
        << QString("enum")
        << QString("event")
        << QString("eventually")
        << QString("expect")
        << QString("export")
        << QString("extends")
        << QString("extern")
        << QString("final")
        << QString("first_match")
        << QString("for")
        << QString("force")
        << QString("foreach")
        << QString("forever")
        << QString("fork")
        << QString("forkjoin")
        << QString("function")
        << QString("generate")
        << QString("genvar")
        << QString("global")
        << QString("highz0")
        << QString("highz1")
        << QString("if")
        << QString("iff")
        << QString("ifnone")
        << QString("ignore_bins")
        << QString("illegal_bins")
        << QString("implements")
        << QString("implies")
        << QString("import")
        << QString("incdir")
        << QString("include")
        << QString("initial")
        << QString("inout")
        << QString("input")
        << QString("inside")
        << QString("instance")
        << QString("int")
        << QString("integer")
        << QString("interconnect")
        << QString("interface")
        << QString("intersect")
        << QString("join")
        << QString("join_any")
        << QString("join_none")
        << QString("large")
        << QString("let")
        << QString("liblist")
        << QString("library")
        << QString("local")
        << QString("localparam")
        << QString("longint")
        << QString("macromodule")
        << QString("matches")
        << QString("medium")
        << QString("modport")
        << QString("module")
        << QString("nand")
        << QString("negedge")
        << QString("nettype")
        << QString("new")
        << QString("nexttime")
        << QString("nmos")
        << QString("nor")
        << QString("noshowcancelled")
        << QString("not")
        << QString("notif0")
        << QString("notif1")
        << QString("null")
        << QString("or")
        << QString("output")
        << QString("package")
        << QString("packed")
        << QString("parameter")
        << QString("pmos")
        << QString("posedge")
        << QString("primitive")
        << QString("priority")
        << QString("program")
        << QString("property")
        << QString("protected")
        << QString("pull0")
        << QString("pull1")
        << QString("pulldown")
        << QString("pullup")
        << QString("pulsestyle_ondetect")
        << QString("pulsestyle_onevent")
        << QString("pure")
        << QString("rand")
        << QString("randc")
        << QString("randcase")
        << QString("randsequence")
        << QString("rcmos")
        << QString("real")
        << QString("realtime")
        << QString("ref")
        << QString("reject_on")
        << QString("release")
        << QString("repeat")
        << QString("restrict")
        << QString("return")
        << QString("rnmos")
        << QString("rpmos")
        << QString("rtran")
        << QString("rtranif0")
        << QString("rtranif1")
        << QString("s_always")
        << QString("s_eventually")
        << QString("s_nexttime")
        << QString("s_until")
        << QString("s_until_with")
        << QString("scalared")
        << QString("sequence")
        << QString("shortint")
        << QString("shortreal")
        << QString("showcancelled")
        << QString("signed")
        << QString("small")
        << QString("soft")
        << QString("solve")
        << QString("specify")
        << QString("specparam")
        << QString("static")
        << QString("string")
        << QString("strong")
        << QString("strong0")
        << QString("strong1")
        << QString("struct")
        << QString("super")
        << QString("supply0")
        << QString("supply1")
        << QString("sync_accept_on")
        << QString("sync_reject_on")
        << QString("table")
        << QString("tagged")
        << QString("task")
        << QString("this")
        << QString("throughout")
        << QString("time")
        << QString("timeprecision")
        << QString("timeunit")
        << QString("tran")
        << QString("tranif0")
        << QString("tranif1")
        << QString("tri")
        << QString("tri0")
        << QString("tri1")
        << QString("triand")
        << QString("trior")
        << QString("trireg")
        << QString("type")
        << QString("typedef")
        << QString("union")
        << QString("unique")
        << QString("unique0")
        << QString("unsigned")
        << QString("until")
        << QString("until_with")
        << QString("untyped")
        << QString("use")
        << QString("uwire")
        << QString("var")
        << QString("vectored")
        << QString("virtual")
        << QString("void")
        << QString("wait")
        << QString("wait_order")
        << QString("wand")
        << QString("weak")
        << QString("weak0")
        << QString("weak1")
        << QString("while")
        << QString("wildcard")
        << QString("with")
        << QString("within")
        << QString("wor")
        << QString("xnor")
        << QString("xor"));

    QStringList sigs(QStringList()
        << QString("logic")
        << QString("reg")
        << QString("wire"));

    QStringList symbols(QStringList()
        << QString("<")
        << QString(">")
        << QString(":")
        << QString("&")
        << QString("%")
        << QString("|")
        << QString("^")
        << QString("~")
        << QString("!")
        << QString("#")
        << QString("+")
        << QString("-")
        << QString("/")
        << QString("*")
        << QString("{")
        << QString("}")
        << QString("="));

    foreach(QString keyword, keywords)
    {
        mKeywordPatterns.append(QRegExp(QString("\\b%1\\b").arg(
            QRegExp::escape(keyword))));
    }

    foreach(QString symbol, symbols)
        mKeywordPatterns.append(QRegExp(QRegExp::escape(symbol)));

    foreach(QString sig, sigs)
    {
        mSignalPatterns.append(QRegExp(QString("\\b%1\\b").arg(
            QRegExp::escape(sig))));
    }
}

VerilogHighlighter::~VerilogHighlighter()
{
    mKeywordPatterns.clear();
    mSignalPatterns.clear();
}

void VerilogHighlighter::highlightBlock(const QString& text)
{
    foreach(QRegExp pattern, mKeywordPatterns)
    {
        int index = text.indexOf(pattern);
        while (index >= 0) {
            int length = pattern.matchedLength();
            setFormat(index, length, mKeywordFormat);
            index = text.indexOf(pattern, index + length);
        }
    }

    foreach(QRegExp pattern, mSignalPatterns)
    {
        int index = text.indexOf(pattern);
        while (index >= 0) {
            int length = pattern.matchedLength();
            setFormat(index, length, mSignalFormat);
            index = text.indexOf(pattern, index + length);
        }
    }

    {
        int index = text.indexOf(mIdentPattern);
        while (index >= 0) {
            int length = mIdentPattern.matchedLength();
            setFormat(index, length, mIdentFormat);
            index = text.indexOf(mIdentPattern, index + length);
        }
    }

    {
        int index = text.indexOf(mNumberPattern);
        while (index >= 0) {
            int length = mNumberPattern.matchedLength();
            setFormat(index, length, mNumberFormat);
            index = text.indexOf(mNumberPattern, index + length);
        }
    }

    {
        int index = text.indexOf(mSingleCommentPattern);
        while (index >= 0) {
            int length = mSingleCommentPattern.matchedLength();
            setFormat(index, length, mCommentFormat);
            index = text.indexOf(mSingleCommentPattern, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if(previousBlockState() != 1)
        startIndex = text.indexOf(mStartCommentPattern);

    while(startIndex >= 0) {
        int endIndex = text.indexOf(mEndCommentPattern, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                + mEndCommentPattern.matchedLength();
        }
        setFormat(startIndex, commentLength, mCommentFormat);
        startIndex = text.indexOf(mStartCommentPattern,
            startIndex + commentLength);
    }
}
