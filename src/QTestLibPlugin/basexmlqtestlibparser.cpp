/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
 *
 * This file is part of QTestLibPlugin.
 *
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#include "basexmlqtestlibparser.h"

#include <QXmlStreamReader>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

BaseXMLQTestLibParser::BaseXMLQTestLibParser(QObject *parent) :
    AbstractTestParser(parent), mModel(NULL), mParserActive(false)
{
    mReader = new QXmlStreamReader();

    mXmlStarts.clear();
    mXmlStarts << QLatin1String("<?xml");
}

TestModelFactory::ParseResult BaseXMLQTestLibParser::parseStdoutLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    TestModelFactory::ParseResult result = TestModelFactory::Unsure;

    QString cleanedLine = line.trimmed() + QLatin1Char('\n');
    if (!mParserActive) {
        foreach (QString xmlStart, mXmlStarts) {
            if (cleanedLine.contains(xmlStart)) {
                mParserActive = true;
                cleanedLine = cleanedLine.mid(cleanedLine.indexOf(xmlStart, 0));
                break;
            }
        }
    }

    if (!mParserActive)
        return result;
    mReader->addData(cleanedLine);

    while(!mReader->atEnd()) {
        QXmlStreamReader::TokenType currentToken = mReader->readNext();

        if (currentToken == QXmlStreamReader::Invalid) {
            if (mReader->error() == QXmlStreamReader::PrematureEndOfDocumentError)
                return result;
            else
                return TestModelFactory::MagicNotFound;
        }

        if (mCurrentElement.isEmpty()) {
            if ((currentToken != QXmlStreamReader::StartDocument)
             && (currentToken != QXmlStreamReader::EndDocument)
             && (currentToken != QXmlStreamReader::StartElement))
                return TestModelFactory::MagicNotFound;
        }

        switch(currentToken) {
        case QXmlStreamReader::NoToken:
            break;
        case QXmlStreamReader::Invalid:
            Q_ASSERT_X(false, "Unreachable place", "The program cannot reach this point");
        case QXmlStreamReader::StartDocument:
            break;
        case QXmlStreamReader::EndDocument:
            mReader->clear();
            mParserActive = false;
            qDebug() << "End of document";
            break;
        case QXmlStreamReader::StartElement:
            mCurrentElement.push(mReader->name().toString());
            result = qMax(result, startElementParsed(runControl, mReader->name()));
            break;
        case QXmlStreamReader::EndElement:
            if (QStringRef::compare(mReader->name(), mCurrentElement.pop(), Qt::CaseSensitive) != 0)
                return TestModelFactory::MagicNotFound;
            result = qMax(result, endElementParsed(runControl, mReader->name()));
            break;
        case QXmlStreamReader::Characters:
            result = qMax(result, textParsed(runControl));
            break;
        case QXmlStreamReader::Comment:
            result = qMax(result, commentParsed(runControl));
            qDebug() << "Comment";
            break;
        case QXmlStreamReader::DTD:
            qDebug() << "Seen DTD";
            break;
        case QXmlStreamReader::EntityReference:
            qDebug() << "Entity reference";
            break;
        case QXmlStreamReader::ProcessingInstruction:
            qDebug() << "processing instruction";
            break;
        }
    }

    return result;
}

TestModelFactory::ParseResult BaseXMLQTestLibParser::parseStderrLine(ProjectExplorer::RunControl* runControl, const QString& line)
{
    Q_UNUSED(runControl);
    Q_UNUSED(line);

    return TestModelFactory::Unsure;
}

void BaseXMLQTestLibParser::saveAttributes(const QXmlStreamAttributes& attrs)
{
    Q_ASSERT(mCurrentAttributes.isEmpty());

    foreach (QXmlStreamAttribute attr, attrs) {
        Q_ASSERT(!mCurrentAttributes.contains(attr.name().toString()));
        mCurrentAttributes.insert(attr.name().toString(), attr.value().toString());
    }
}

QTestLibModel::MessageType BaseXMLQTestLibParser::currentMessageType(void)
{
    if (!mCurrentAttributes.isEmpty()) {
        if (mCurrentAttributes.contains(QLatin1String("type")))
            return messageType(mCurrentAttributes.value(QLatin1String("type")));
        if (mCurrentAttributes.contains(QLatin1String("result")))
            return messageType(mCurrentAttributes.value(QLatin1String("result")));
    } else {
        if (mReader->attributes().hasAttribute(QLatin1String("result")))
            return messageType(mReader->attributes().value(QLatin1String("result")).toString());
        if (mReader->attributes().hasAttribute(QLatin1String("type")))
            return messageType(mReader->attributes().value(QLatin1String("type")).toString());
    }
    qWarning() << "Message type did not find a type";
    return QTestLibModel::Unknown;
}

QTestLibModel::MessageType BaseXMLQTestLibParser::messageType(const QString& messageType)
{
    int type = -1;
    const QString messages(QLatin1String("result  "
                                         "qdebug  "
                                         "info    "
                                         "warn    "
                                         "qwarn   "
                                         "system  "
                                         "qfatal  "
                                         "??????  "
                                         "skip    "
                                         "pass    "
                                         "bpass   "
                                         "xpass   "
                                         "xfail   "
                                         "bfail   "
                                         "fail    "));

    Q_ASSERT(messageType.length() <= 8);

    // Find the type of the message
    if (!messageType.isEmpty())
        type = messages.indexOf(messageType.leftJustified(8, QLatin1Char(' '), false), 0, Qt::CaseSensitive);
    if (type == -1)
        type = (int) QTestLibModel::Unknown;
    else
        type = (type >> 3) + 3;

    return (QTestLibModel::MessageType) type;
}

QHash<QString, QString> BaseXMLQTestLibParser::sEntities;

void BaseXMLQTestLibParser::initXMLEntities(void)
{
    sEntities.insert(QLatin1String("AElig"), QLatin1String("Æ"));
    sEntities.insert(QLatin1String("Aacute"), QLatin1String("Á"));
    sEntities.insert(QLatin1String("Acirc"), QLatin1String("Â"));
    sEntities.insert(QLatin1String("Agrave"), QLatin1String("À"));
    sEntities.insert(QLatin1String("Alpha"), QLatin1String("Α"));
    sEntities.insert(QLatin1String("Aring"), QLatin1String("Å"));
    sEntities.insert(QLatin1String("Atilde"), QLatin1String("Ã"));
    sEntities.insert(QLatin1String("Auml"), QLatin1String("Ä"));
    sEntities.insert(QLatin1String("Beta"), QLatin1String("Β"));
    sEntities.insert(QLatin1String("Ccedil"), QLatin1String("Ç"));
    sEntities.insert(QLatin1String("Chi"), QLatin1String("Χ"));
    sEntities.insert(QLatin1String("Dagger"), QLatin1String("‡"));
    sEntities.insert(QLatin1String("Delta"), QLatin1String("Δ"));
    sEntities.insert(QLatin1String("ETH"), QLatin1String("Ð"));
    sEntities.insert(QLatin1String("Eacute"), QLatin1String("É"));
    sEntities.insert(QLatin1String("Ecirc"), QLatin1String("Ê"));
    sEntities.insert(QLatin1String("Egrave"), QLatin1String("È"));
    sEntities.insert(QLatin1String("Epsilon"), QLatin1String("Ε"));
    sEntities.insert(QLatin1String("Eta"), QLatin1String("Η"));
    sEntities.insert(QLatin1String("Euml"), QLatin1String("Ë"));
    sEntities.insert(QLatin1String("Gamma"), QLatin1String("Γ"));
    sEntities.insert(QLatin1String("Iacute"), QLatin1String("Í"));
    sEntities.insert(QLatin1String("Icirc"), QLatin1String("Î"));
    sEntities.insert(QLatin1String("Igrave"), QLatin1String("Ì"));
    sEntities.insert(QLatin1String("Iota"), QLatin1String("Ι"));
    sEntities.insert(QLatin1String("Iuml"), QLatin1String("Ï"));
    sEntities.insert(QLatin1String("Kappa"), QLatin1String("Κ"));
    sEntities.insert(QLatin1String("Lambda"), QLatin1String("Λ"));
    sEntities.insert(QLatin1String("Mu"), QLatin1String("Μ"));
    sEntities.insert(QLatin1String("Ntilde"), QLatin1String("Ñ"));
    sEntities.insert(QLatin1String("Nu"), QLatin1String("Ν"));
    sEntities.insert(QLatin1String("OElig"), QLatin1String("Œ"));
    sEntities.insert(QLatin1String("Oacute"), QLatin1String("Ó"));
    sEntities.insert(QLatin1String("Ocirc"), QLatin1String("Ô"));
    sEntities.insert(QLatin1String("Ograve"), QLatin1String("Ò"));
    sEntities.insert(QLatin1String("Omega"), QLatin1String("Ω"));
    sEntities.insert(QLatin1String("Omicron"), QLatin1String("Ο"));
    sEntities.insert(QLatin1String("Oslash"), QLatin1String("Ø"));
    sEntities.insert(QLatin1String("Otilde"), QLatin1String("Õ"));
    sEntities.insert(QLatin1String("Ouml"), QLatin1String("Ö"));
    sEntities.insert(QLatin1String("Phi"), QLatin1String("Φ"));
    sEntities.insert(QLatin1String("Pi"), QLatin1String("Π"));
    sEntities.insert(QLatin1String("Prime"), QLatin1String("″"));
    sEntities.insert(QLatin1String("Psi"), QLatin1String("Ψ"));
    sEntities.insert(QLatin1String("Rho"), QLatin1String("Ρ"));
    sEntities.insert(QLatin1String("Scaron"), QLatin1String("Š"));
    sEntities.insert(QLatin1String("Sigma"), QLatin1String("Σ"));
    sEntities.insert(QLatin1String("THORN"), QLatin1String("Þ"));
    sEntities.insert(QLatin1String("Tau"), QLatin1String("Τ"));
    sEntities.insert(QLatin1String("Theta"), QLatin1String("Θ"));
    sEntities.insert(QLatin1String("Uacute"), QLatin1String("Ú"));
    sEntities.insert(QLatin1String("Ucirc"), QLatin1String("Û"));
    sEntities.insert(QLatin1String("Ugrave"), QLatin1String("Ù"));
    sEntities.insert(QLatin1String("Upsilon"), QLatin1String("Υ"));
    sEntities.insert(QLatin1String("Uuml"), QLatin1String("Ü"));
    sEntities.insert(QLatin1String("Xi"), QLatin1String("Ξ"));
    sEntities.insert(QLatin1String("Yacute"), QLatin1String("Ý"));
    sEntities.insert(QLatin1String("Yuml"), QLatin1String("Ÿ"));
    sEntities.insert(QLatin1String("Zeta"), QLatin1String("Ζ"));
    sEntities.insert(QLatin1String("aacute"), QLatin1String("á"));
    sEntities.insert(QLatin1String("acirc"), QLatin1String("â"));
    sEntities.insert(QLatin1String("acute"), QLatin1String("´"));
    sEntities.insert(QLatin1String("aelig"), QLatin1String("æ"));
    sEntities.insert(QLatin1String("agrave"), QLatin1String("à"));
    sEntities.insert(QLatin1String("alefsym"), QLatin1String("ℵ"));
    sEntities.insert(QLatin1String("alpha"), QLatin1String("α"));
    sEntities.insert(QLatin1String("amp"), QLatin1String("&"));
    sEntities.insert(QLatin1String("and"), QLatin1String("∧"));
    sEntities.insert(QLatin1String("ang"), QLatin1String("∠"));
    sEntities.insert(QLatin1String("apos"), QLatin1String("'"));
    sEntities.insert(QLatin1String("aring"), QLatin1String("å"));
    sEntities.insert(QLatin1String("asymp"), QLatin1String("≈"));
    sEntities.insert(QLatin1String("atilde"), QLatin1String("ã"));
    sEntities.insert(QLatin1String("auml"), QLatin1String("ä"));
    sEntities.insert(QLatin1String("bdquo"), QLatin1String("„"));
    sEntities.insert(QLatin1String("beta"), QLatin1String("β"));
    sEntities.insert(QLatin1String("brvbar"), QLatin1String("¦"));
    sEntities.insert(QLatin1String("bull"), QLatin1String("•"));
    sEntities.insert(QLatin1String("cap"), QLatin1String("∩"));
    sEntities.insert(QLatin1String("ccedil"), QLatin1String("ç"));
    sEntities.insert(QLatin1String("cedil"), QLatin1String("¸"));
    sEntities.insert(QLatin1String("cent"), QLatin1String("¢"));
    sEntities.insert(QLatin1String("chi"), QLatin1String("χ"));
    sEntities.insert(QLatin1String("circ"), QLatin1String("ˆ"));
    sEntities.insert(QLatin1String("clubs"), QLatin1String("♣"));
    sEntities.insert(QLatin1String("cong"), QLatin1String("≅"));
    sEntities.insert(QLatin1String("copy"), QLatin1String("©"));
    sEntities.insert(QLatin1String("crarr"), QLatin1String("↵"));
    sEntities.insert(QLatin1String("cup"), QLatin1String("∪"));
    sEntities.insert(QLatin1String("curren"), QLatin1String("¤"));
    sEntities.insert(QLatin1String("dArr"), QLatin1String("⇓"));
    sEntities.insert(QLatin1String("dagger"), QLatin1String("†"));
    sEntities.insert(QLatin1String("darr"), QLatin1String("↓"));
    sEntities.insert(QLatin1String("deg"), QLatin1String("°"));
    sEntities.insert(QLatin1String("delta"), QLatin1String("δ"));
    sEntities.insert(QLatin1String("diams"), QLatin1String("♦"));
    sEntities.insert(QLatin1String("divide"), QLatin1String("÷"));
    sEntities.insert(QLatin1String("eacute"), QLatin1String("é"));
    sEntities.insert(QLatin1String("ecirc"), QLatin1String("ê"));
    sEntities.insert(QLatin1String("egrave"), QLatin1String("è"));
    sEntities.insert(QLatin1String("empty"), QLatin1String("∅"));
    sEntities.insert(QLatin1String("emsp"), QLatin1String(" "));
    sEntities.insert(QLatin1String("ensp"), QLatin1String(" "));
    sEntities.insert(QLatin1String("epsilon"), QLatin1String("ε"));
    sEntities.insert(QLatin1String("equiv"), QLatin1String("≡"));
    sEntities.insert(QLatin1String("eta"), QLatin1String("η"));
    sEntities.insert(QLatin1String("eth"), QLatin1String("ð"));
    sEntities.insert(QLatin1String("euml"), QLatin1String("ë"));
    sEntities.insert(QLatin1String("euro"), QLatin1String("€"));
    sEntities.insert(QLatin1String("exist"), QLatin1String("∃"));
    sEntities.insert(QLatin1String("fnof"), QLatin1String("ƒ"));
    sEntities.insert(QLatin1String("forall"), QLatin1String("∀"));
    sEntities.insert(QLatin1String("frac12"), QLatin1String("½"));
    sEntities.insert(QLatin1String("frac14"), QLatin1String("¼"));
    sEntities.insert(QLatin1String("frac34"), QLatin1String("¾"));
    sEntities.insert(QLatin1String("frasl"), QLatin1String("⁄"));
    sEntities.insert(QLatin1String("gamma"), QLatin1String("γ"));
    sEntities.insert(QLatin1String("ge"), QLatin1String("≥"));
    sEntities.insert(QLatin1String("gt"), QLatin1String(">"));
    sEntities.insert(QLatin1String("hArr"), QLatin1String("⇔"));
    sEntities.insert(QLatin1String("harr"), QLatin1String("↔"));
    sEntities.insert(QLatin1String("hearts"), QLatin1String("♥"));
    sEntities.insert(QLatin1String("hellip"), QLatin1String("…"));
    sEntities.insert(QLatin1String("iacute"), QLatin1String("í"));
    sEntities.insert(QLatin1String("icirc"), QLatin1String("î"));
    sEntities.insert(QLatin1String("iexcl"), QLatin1String("¡"));
    sEntities.insert(QLatin1String("igrave"), QLatin1String("ì"));
    sEntities.insert(QLatin1String("image"), QLatin1String("ℑ"));
    sEntities.insert(QLatin1String("infin"), QLatin1String("∞"));
    sEntities.insert(QLatin1String("int"), QLatin1String("∫"));
    sEntities.insert(QLatin1String("iota"), QLatin1String("ι"));
    sEntities.insert(QLatin1String("iquest"), QLatin1String("¿"));
    sEntities.insert(QLatin1String("isin"), QLatin1String("∈"));
    sEntities.insert(QLatin1String("iuml"), QLatin1String("ï"));
    sEntities.insert(QLatin1String("kappa"), QLatin1String("κ"));
    sEntities.insert(QLatin1String("lArr"), QLatin1String("⇐"));
    sEntities.insert(QLatin1String("lambda"), QLatin1String("λ"));
    sEntities.insert(QLatin1String("lang"), QLatin1String("〈"));
    sEntities.insert(QLatin1String("laquo"), QLatin1String("«"));
    sEntities.insert(QLatin1String("larr"), QLatin1String("←"));
    sEntities.insert(QLatin1String("lceil"), QLatin1String("⌈"));
    sEntities.insert(QLatin1String("ldquo"), QLatin1String("“"));
    sEntities.insert(QLatin1String("le"), QLatin1String("≤"));
    sEntities.insert(QLatin1String("lfloor"), QLatin1String("⌊"));
    sEntities.insert(QLatin1String("lowast"), QLatin1String("∗"));
    sEntities.insert(QLatin1String("loz"), QLatin1String("◊"));
    sEntities.insert(QLatin1String("lrm"), QLatin1String("\xE2\x80\x8E"));
    sEntities.insert(QLatin1String("lsaquo"), QLatin1String("‹"));
    sEntities.insert(QLatin1String("lsquo"), QLatin1String("‘"));
    sEntities.insert(QLatin1String("lt"), QLatin1String("<"));
    sEntities.insert(QLatin1String("macr"), QLatin1String("¯"));
    sEntities.insert(QLatin1String("mdash"), QLatin1String("—"));
    sEntities.insert(QLatin1String("micro"), QLatin1String("µ"));
    sEntities.insert(QLatin1String("middot"), QLatin1String("·"));
    sEntities.insert(QLatin1String("minus"), QLatin1String("−"));
    sEntities.insert(QLatin1String("mu"), QLatin1String("μ"));
    sEntities.insert(QLatin1String("nabla"), QLatin1String("∇"));
    sEntities.insert(QLatin1String("nbsp"), QLatin1String(" "));
    sEntities.insert(QLatin1String("ndash"), QLatin1String("–"));
    sEntities.insert(QLatin1String("ne"), QLatin1String("≠"));
    sEntities.insert(QLatin1String("ni"), QLatin1String("∋"));
    sEntities.insert(QLatin1String("not"), QLatin1String("¬"));
    sEntities.insert(QLatin1String("notin"), QLatin1String("∉"));
    sEntities.insert(QLatin1String("nsub"), QLatin1String("⊄"));
    sEntities.insert(QLatin1String("ntilde"), QLatin1String("ñ"));
    sEntities.insert(QLatin1String("nu"), QLatin1String("ν"));
    sEntities.insert(QLatin1String("oacute"), QLatin1String("ó"));
    sEntities.insert(QLatin1String("ocirc"), QLatin1String("ô"));
    sEntities.insert(QLatin1String("oelig"), QLatin1String("œ"));
    sEntities.insert(QLatin1String("ograve"), QLatin1String("ò"));
    sEntities.insert(QLatin1String("oline"), QLatin1String("‾"));
    sEntities.insert(QLatin1String("omega"), QLatin1String("ω"));
    sEntities.insert(QLatin1String("omicron"), QLatin1String("ο"));
    sEntities.insert(QLatin1String("oplus"), QLatin1String("⊕"));
    sEntities.insert(QLatin1String("or"), QLatin1String("∨"));
    sEntities.insert(QLatin1String("ordf"), QLatin1String("ª"));
    sEntities.insert(QLatin1String("ordm"), QLatin1String("º"));
    sEntities.insert(QLatin1String("oslash"), QLatin1String("ø"));
    sEntities.insert(QLatin1String("otilde"), QLatin1String("õ"));
    sEntities.insert(QLatin1String("otimes"), QLatin1String("⊗"));
    sEntities.insert(QLatin1String("ouml"), QLatin1String("ö"));
    sEntities.insert(QLatin1String("para"), QLatin1String("¶"));
    sEntities.insert(QLatin1String("part"), QLatin1String("∂"));
    sEntities.insert(QLatin1String("permil"), QLatin1String("‰"));
    sEntities.insert(QLatin1String("perp"), QLatin1String("⊥"));
    sEntities.insert(QLatin1String("phi"), QLatin1String("φ"));
    sEntities.insert(QLatin1String("pi"), QLatin1String("π"));
    sEntities.insert(QLatin1String("piv"), QLatin1String("ϖ"));
    sEntities.insert(QLatin1String("plusmn"), QLatin1String("±"));
    sEntities.insert(QLatin1String("pound"), QLatin1String("£"));
    sEntities.insert(QLatin1String("prime"), QLatin1String("′"));
    sEntities.insert(QLatin1String("prod"), QLatin1String("∏"));
    sEntities.insert(QLatin1String("prop"), QLatin1String("∝"));
    sEntities.insert(QLatin1String("psi"), QLatin1String("ψ"));
    sEntities.insert(QLatin1String("quot"), QLatin1String("\""));
    sEntities.insert(QLatin1String("rArr"), QLatin1String("⇒"));
    sEntities.insert(QLatin1String("radic"), QLatin1String("√"));
    sEntities.insert(QLatin1String("rang"), QLatin1String("〉"));
    sEntities.insert(QLatin1String("raquo"), QLatin1String("»"));
    sEntities.insert(QLatin1String("rarr"), QLatin1String("→"));
    sEntities.insert(QLatin1String("rceil"), QLatin1String("⌉"));
    sEntities.insert(QLatin1String("rdquo"), QLatin1String("”"));
    sEntities.insert(QLatin1String("real"), QLatin1String("ℜ"));
    sEntities.insert(QLatin1String("reg"), QLatin1String("®"));
    sEntities.insert(QLatin1String("rfloor"), QLatin1String("⌋"));
    sEntities.insert(QLatin1String("rho"), QLatin1String("ρ"));
    sEntities.insert(QLatin1String("rlm"), QLatin1String("\xE2\x80\x8F"));
    sEntities.insert(QLatin1String("rsaquo"), QLatin1String("›"));
    sEntities.insert(QLatin1String("rsquo"), QLatin1String("’"));
    sEntities.insert(QLatin1String("sbquo"), QLatin1String("‚"));
    sEntities.insert(QLatin1String("scaron"), QLatin1String("š"));
    sEntities.insert(QLatin1String("sdot"), QLatin1String("⋅"));
    sEntities.insert(QLatin1String("sect"), QLatin1String("§"));
    sEntities.insert(QLatin1String("shy"), QLatin1String("\xC2\xAD"));
    sEntities.insert(QLatin1String("sigma"), QLatin1String("σ"));
    sEntities.insert(QLatin1String("sigmaf"), QLatin1String("ς"));
    sEntities.insert(QLatin1String("sim"), QLatin1String("∼"));
    sEntities.insert(QLatin1String("spades"), QLatin1String("♠"));
    sEntities.insert(QLatin1String("sub"), QLatin1String("⊂"));
    sEntities.insert(QLatin1String("sube"), QLatin1String("⊆"));
    sEntities.insert(QLatin1String("sum"), QLatin1String("∑"));
    sEntities.insert(QLatin1String("sup1"), QLatin1String("¹"));
    sEntities.insert(QLatin1String("sup2"), QLatin1String("²"));
    sEntities.insert(QLatin1String("sup3"), QLatin1String("³"));
    sEntities.insert(QLatin1String("sup"), QLatin1String("⊃"));
    sEntities.insert(QLatin1String("supe"), QLatin1String("⊇"));
    sEntities.insert(QLatin1String("szlig"), QLatin1String("ß"));
    sEntities.insert(QLatin1String("tau"), QLatin1String("τ"));
    sEntities.insert(QLatin1String("there4"), QLatin1String("∴"));
    sEntities.insert(QLatin1String("theta"), QLatin1String("θ"));
    sEntities.insert(QLatin1String("thetasym"), QLatin1String("ϑ"));
    sEntities.insert(QLatin1String("thinsp"), QLatin1String(" "));
    sEntities.insert(QLatin1String("thorn"), QLatin1String("þ"));
    sEntities.insert(QLatin1String("tilde"), QLatin1String("˜"));
    sEntities.insert(QLatin1String("times"), QLatin1String("×"));
    sEntities.insert(QLatin1String("trade"), QLatin1String("™"));
    sEntities.insert(QLatin1String("uArr"), QLatin1String("⇑"));
    sEntities.insert(QLatin1String("uacute"), QLatin1String("ú"));
    sEntities.insert(QLatin1String("uarr"), QLatin1String("↑"));
    sEntities.insert(QLatin1String("ucirc"), QLatin1String("û"));
    sEntities.insert(QLatin1String("ugrave"), QLatin1String("ù"));
    sEntities.insert(QLatin1String("uml"), QLatin1String("¨"));
    sEntities.insert(QLatin1String("upsih"), QLatin1String("ϒ"));
    sEntities.insert(QLatin1String("upsilon"), QLatin1String("υ"));
    sEntities.insert(QLatin1String("uuml"), QLatin1String("ü"));
    sEntities.insert(QLatin1String("weierp"), QLatin1String("℘"));
    sEntities.insert(QLatin1String("xi"), QLatin1String("ξ"));
    sEntities.insert(QLatin1String("yacute"), QLatin1String("ý"));
    sEntities.insert(QLatin1String("yen"), QLatin1String("¥"));
    sEntities.insert(QLatin1String("yuml"), QLatin1String("ÿ"));
    sEntities.insert(QLatin1String("zeta"), QLatin1String("ζ"));
    sEntities.insert(QLatin1String("zwj"), QLatin1String("\xE2\x80\x8D"));
    sEntities.insert(QLatin1String("zwnj"), QLatin1String("\xE2\x80\x8C"));
}

QString BaseXMLQTestLibParser::decodeXMLEntities(const QString& encoded)
{
    QString decoded;
    decoded.reserve(encoded.length());

    if (sEntities.isEmpty())
        initXMLEntities();

    int d = 0;
    for (int c = 0; c < encoded.length(); c++) {
        if ((encoded[c] != QLatin1Char('&')) || (c == encoded.length() - 1)) {
            decoded.replace(d++, 1, encoded[c]);
            continue;
        }

        int b = c + 1;
        c = encoded.indexOf(QLatin1Char(';'), c);
        if (c == -1) {
            decoded.replace(d++, 1, QLatin1Char('&'));
            c = b - 1;
            continue;
        }

        QString entity = encoded.mid(b, c - b);

        if (entity.startsWith(QLatin1String("#x"), Qt::CaseInsensitive)) {
            entity.remove(0, 2);
            decoded.replace(d++, 1, QChar(entity.toShort(NULL, 16)));
        } else if (entity.startsWith(QLatin1Char('#'))) {
            Q_ASSERT_X(false, __func__, "Unsupported XML entity");
        } else {
            if (sEntities.contains(entity))
                decoded.replace(d++, 1, sEntities.value(entity));
            else
                Q_ASSERT_X(false, __func__, qPrintable(QString(QLatin1String("Unknown XML entity: %1")).arg(entity)));
        }
    }

    decoded.squeeze();

    qDebug() << __func__ << encoded << decoded;
    return decoded;
}

} // namespace Internal
} // namespace QTestLibPlugin
