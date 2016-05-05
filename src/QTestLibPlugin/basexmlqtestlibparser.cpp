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
    sEntities.insert(QLatin1String("AElig"), QStringLiteral("Æ"));
    sEntities.insert(QLatin1String("Aacute"), QStringLiteral("Á"));
    sEntities.insert(QLatin1String("Acirc"), QStringLiteral("Â"));
    sEntities.insert(QLatin1String("Agrave"), QStringLiteral("À"));
    sEntities.insert(QLatin1String("Alpha"), QStringLiteral("Α"));
    sEntities.insert(QLatin1String("Aring"), QStringLiteral("Å"));
    sEntities.insert(QLatin1String("Atilde"), QStringLiteral("Ã"));
    sEntities.insert(QLatin1String("Auml"), QStringLiteral("Ä"));
    sEntities.insert(QLatin1String("Beta"), QStringLiteral("Β"));
    sEntities.insert(QLatin1String("Ccedil"), QStringLiteral("Ç"));
    sEntities.insert(QLatin1String("Chi"), QStringLiteral("Χ"));
    sEntities.insert(QLatin1String("Dagger"), QStringLiteral("‡"));
    sEntities.insert(QLatin1String("Delta"), QStringLiteral("Δ"));
    sEntities.insert(QLatin1String("ETH"), QStringLiteral("Ð"));
    sEntities.insert(QLatin1String("Eacute"), QStringLiteral("É"));
    sEntities.insert(QLatin1String("Ecirc"), QStringLiteral("Ê"));
    sEntities.insert(QLatin1String("Egrave"), QStringLiteral("È"));
    sEntities.insert(QLatin1String("Epsilon"), QStringLiteral("Ε"));
    sEntities.insert(QLatin1String("Eta"), QStringLiteral("Η"));
    sEntities.insert(QLatin1String("Euml"), QStringLiteral("Ë"));
    sEntities.insert(QLatin1String("Gamma"), QStringLiteral("Γ"));
    sEntities.insert(QLatin1String("Iacute"), QStringLiteral("Í"));
    sEntities.insert(QLatin1String("Icirc"), QStringLiteral("Î"));
    sEntities.insert(QLatin1String("Igrave"), QStringLiteral("Ì"));
    sEntities.insert(QLatin1String("Iota"), QStringLiteral("Ι"));
    sEntities.insert(QLatin1String("Iuml"), QStringLiteral("Ï"));
    sEntities.insert(QLatin1String("Kappa"), QStringLiteral("Κ"));
    sEntities.insert(QLatin1String("Lambda"), QStringLiteral("Λ"));
    sEntities.insert(QLatin1String("Mu"), QStringLiteral("Μ"));
    sEntities.insert(QLatin1String("Ntilde"), QStringLiteral("Ñ"));
    sEntities.insert(QLatin1String("Nu"), QStringLiteral("Ν"));
    sEntities.insert(QLatin1String("OElig"), QStringLiteral("Œ"));
    sEntities.insert(QLatin1String("Oacute"), QStringLiteral("Ó"));
    sEntities.insert(QLatin1String("Ocirc"), QStringLiteral("Ô"));
    sEntities.insert(QLatin1String("Ograve"), QStringLiteral("Ò"));
    sEntities.insert(QLatin1String("Omega"), QStringLiteral("Ω"));
    sEntities.insert(QLatin1String("Omicron"), QStringLiteral("Ο"));
    sEntities.insert(QLatin1String("Oslash"), QStringLiteral("Ø"));
    sEntities.insert(QLatin1String("Otilde"), QStringLiteral("Õ"));
    sEntities.insert(QLatin1String("Ouml"), QStringLiteral("Ö"));
    sEntities.insert(QLatin1String("Phi"), QStringLiteral("Φ"));
    sEntities.insert(QLatin1String("Pi"), QStringLiteral("Π"));
    sEntities.insert(QLatin1String("Prime"), QStringLiteral("″"));
    sEntities.insert(QLatin1String("Psi"), QStringLiteral("Ψ"));
    sEntities.insert(QLatin1String("Rho"), QStringLiteral("Ρ"));
    sEntities.insert(QLatin1String("Scaron"), QStringLiteral("Š"));
    sEntities.insert(QLatin1String("Sigma"), QStringLiteral("Σ"));
    sEntities.insert(QLatin1String("THORN"), QStringLiteral("Þ"));
    sEntities.insert(QLatin1String("Tau"), QStringLiteral("Τ"));
    sEntities.insert(QLatin1String("Theta"), QStringLiteral("Θ"));
    sEntities.insert(QLatin1String("Uacute"), QStringLiteral("Ú"));
    sEntities.insert(QLatin1String("Ucirc"), QStringLiteral("Û"));
    sEntities.insert(QLatin1String("Ugrave"), QStringLiteral("Ù"));
    sEntities.insert(QLatin1String("Upsilon"), QStringLiteral("Υ"));
    sEntities.insert(QLatin1String("Uuml"), QStringLiteral("Ü"));
    sEntities.insert(QLatin1String("Xi"), QStringLiteral("Ξ"));
    sEntities.insert(QLatin1String("Yacute"), QStringLiteral("Ý"));
    sEntities.insert(QLatin1String("Yuml"), QStringLiteral("Ÿ"));
    sEntities.insert(QLatin1String("Zeta"), QStringLiteral("Ζ"));
    sEntities.insert(QLatin1String("aacute"), QStringLiteral("á"));
    sEntities.insert(QLatin1String("acirc"), QStringLiteral("â"));
    sEntities.insert(QLatin1String("acute"), QStringLiteral("´"));
    sEntities.insert(QLatin1String("aelig"), QStringLiteral("æ"));
    sEntities.insert(QLatin1String("agrave"), QStringLiteral("à"));
    sEntities.insert(QLatin1String("alefsym"), QStringLiteral("ℵ"));
    sEntities.insert(QLatin1String("alpha"), QStringLiteral("α"));
    sEntities.insert(QLatin1String("amp"), QStringLiteral("&"));
    sEntities.insert(QLatin1String("and"), QStringLiteral("∧"));
    sEntities.insert(QLatin1String("ang"), QStringLiteral("∠"));
    sEntities.insert(QLatin1String("apos"), QStringLiteral("'"));
    sEntities.insert(QLatin1String("aring"), QStringLiteral("å"));
    sEntities.insert(QLatin1String("asymp"), QStringLiteral("≈"));
    sEntities.insert(QLatin1String("atilde"), QStringLiteral("ã"));
    sEntities.insert(QLatin1String("auml"), QStringLiteral("ä"));
    sEntities.insert(QLatin1String("bdquo"), QStringLiteral("„"));
    sEntities.insert(QLatin1String("beta"), QStringLiteral("β"));
    sEntities.insert(QLatin1String("brvbar"), QStringLiteral("¦"));
    sEntities.insert(QLatin1String("bull"), QStringLiteral("•"));
    sEntities.insert(QLatin1String("cap"), QStringLiteral("∩"));
    sEntities.insert(QLatin1String("ccedil"), QStringLiteral("ç"));
    sEntities.insert(QLatin1String("cedil"), QStringLiteral("¸"));
    sEntities.insert(QLatin1String("cent"), QStringLiteral("¢"));
    sEntities.insert(QLatin1String("chi"), QStringLiteral("χ"));
    sEntities.insert(QLatin1String("circ"), QStringLiteral("ˆ"));
    sEntities.insert(QLatin1String("clubs"), QStringLiteral("♣"));
    sEntities.insert(QLatin1String("cong"), QStringLiteral("≅"));
    sEntities.insert(QLatin1String("copy"), QStringLiteral("©"));
    sEntities.insert(QLatin1String("crarr"), QStringLiteral("↵"));
    sEntities.insert(QLatin1String("cup"), QStringLiteral("∪"));
    sEntities.insert(QLatin1String("curren"), QStringLiteral("¤"));
    sEntities.insert(QLatin1String("dArr"), QStringLiteral("⇓"));
    sEntities.insert(QLatin1String("dagger"), QStringLiteral("†"));
    sEntities.insert(QLatin1String("darr"), QStringLiteral("↓"));
    sEntities.insert(QLatin1String("deg"), QStringLiteral("°"));
    sEntities.insert(QLatin1String("delta"), QStringLiteral("δ"));
    sEntities.insert(QLatin1String("diams"), QStringLiteral("♦"));
    sEntities.insert(QLatin1String("divide"), QStringLiteral("÷"));
    sEntities.insert(QLatin1String("eacute"), QStringLiteral("é"));
    sEntities.insert(QLatin1String("ecirc"), QStringLiteral("ê"));
    sEntities.insert(QLatin1String("egrave"), QStringLiteral("è"));
    sEntities.insert(QLatin1String("empty"), QStringLiteral("∅"));
    sEntities.insert(QLatin1String("emsp"), QStringLiteral(" "));
    sEntities.insert(QLatin1String("ensp"), QStringLiteral(" "));
    sEntities.insert(QLatin1String("epsilon"), QStringLiteral("ε"));
    sEntities.insert(QLatin1String("equiv"), QStringLiteral("≡"));
    sEntities.insert(QLatin1String("eta"), QStringLiteral("η"));
    sEntities.insert(QLatin1String("eth"), QStringLiteral("ð"));
    sEntities.insert(QLatin1String("euml"), QStringLiteral("ë"));
    sEntities.insert(QLatin1String("euro"), QStringLiteral("€"));
    sEntities.insert(QLatin1String("exist"), QStringLiteral("∃"));
    sEntities.insert(QLatin1String("fnof"), QStringLiteral("ƒ"));
    sEntities.insert(QLatin1String("forall"), QStringLiteral("∀"));
    sEntities.insert(QLatin1String("frac12"), QStringLiteral("½"));
    sEntities.insert(QLatin1String("frac14"), QStringLiteral("¼"));
    sEntities.insert(QLatin1String("frac34"), QStringLiteral("¾"));
    sEntities.insert(QLatin1String("frasl"), QStringLiteral("⁄"));
    sEntities.insert(QLatin1String("gamma"), QStringLiteral("γ"));
    sEntities.insert(QLatin1String("ge"), QStringLiteral("≥"));
    sEntities.insert(QLatin1String("gt"), QStringLiteral(">"));
    sEntities.insert(QLatin1String("hArr"), QStringLiteral("⇔"));
    sEntities.insert(QLatin1String("harr"), QStringLiteral("↔"));
    sEntities.insert(QLatin1String("hearts"), QStringLiteral("♥"));
    sEntities.insert(QLatin1String("hellip"), QStringLiteral("…"));
    sEntities.insert(QLatin1String("iacute"), QStringLiteral("í"));
    sEntities.insert(QLatin1String("icirc"), QStringLiteral("î"));
    sEntities.insert(QLatin1String("iexcl"), QStringLiteral("¡"));
    sEntities.insert(QLatin1String("igrave"), QStringLiteral("ì"));
    sEntities.insert(QLatin1String("image"), QStringLiteral("ℑ"));
    sEntities.insert(QLatin1String("infin"), QStringLiteral("∞"));
    sEntities.insert(QLatin1String("int"), QStringLiteral("∫"));
    sEntities.insert(QLatin1String("iota"), QStringLiteral("ι"));
    sEntities.insert(QLatin1String("iquest"), QStringLiteral("¿"));
    sEntities.insert(QLatin1String("isin"), QStringLiteral("∈"));
    sEntities.insert(QLatin1String("iuml"), QStringLiteral("ï"));
    sEntities.insert(QLatin1String("kappa"), QStringLiteral("κ"));
    sEntities.insert(QLatin1String("lArr"), QStringLiteral("⇐"));
    sEntities.insert(QLatin1String("lambda"), QStringLiteral("λ"));
    sEntities.insert(QLatin1String("lang"), QStringLiteral("〈"));
    sEntities.insert(QLatin1String("laquo"), QStringLiteral("«"));
    sEntities.insert(QLatin1String("larr"), QStringLiteral("←"));
    sEntities.insert(QLatin1String("lceil"), QStringLiteral("⌈"));
    sEntities.insert(QLatin1String("ldquo"), QStringLiteral("“"));
    sEntities.insert(QLatin1String("le"), QStringLiteral("≤"));
    sEntities.insert(QLatin1String("lfloor"), QStringLiteral("⌊"));
    sEntities.insert(QLatin1String("lowast"), QStringLiteral("∗"));
    sEntities.insert(QLatin1String("loz"), QStringLiteral("◊"));
    sEntities.insert(QLatin1String("lrm"), QStringLiteral("\xE2\x80\x8E"));
    sEntities.insert(QLatin1String("lsaquo"), QStringLiteral("‹"));
    sEntities.insert(QLatin1String("lsquo"), QStringLiteral("‘"));
    sEntities.insert(QLatin1String("lt"), QStringLiteral("<"));
    sEntities.insert(QLatin1String("macr"), QStringLiteral("¯"));
    sEntities.insert(QLatin1String("mdash"), QStringLiteral("—"));
    sEntities.insert(QLatin1String("micro"), QStringLiteral("µ"));
    sEntities.insert(QLatin1String("middot"), QStringLiteral("·"));
    sEntities.insert(QLatin1String("minus"), QStringLiteral("−"));
    sEntities.insert(QLatin1String("mu"), QStringLiteral("μ"));
    sEntities.insert(QLatin1String("nabla"), QStringLiteral("∇"));
    sEntities.insert(QLatin1String("nbsp"), QStringLiteral(" "));
    sEntities.insert(QLatin1String("ndash"), QStringLiteral("–"));
    sEntities.insert(QLatin1String("ne"), QStringLiteral("≠"));
    sEntities.insert(QLatin1String("ni"), QStringLiteral("∋"));
    sEntities.insert(QLatin1String("not"), QStringLiteral("¬"));
    sEntities.insert(QLatin1String("notin"), QStringLiteral("∉"));
    sEntities.insert(QLatin1String("nsub"), QStringLiteral("⊄"));
    sEntities.insert(QLatin1String("ntilde"), QStringLiteral("ñ"));
    sEntities.insert(QLatin1String("nu"), QStringLiteral("ν"));
    sEntities.insert(QLatin1String("oacute"), QStringLiteral("ó"));
    sEntities.insert(QLatin1String("ocirc"), QStringLiteral("ô"));
    sEntities.insert(QLatin1String("oelig"), QStringLiteral("œ"));
    sEntities.insert(QLatin1String("ograve"), QStringLiteral("ò"));
    sEntities.insert(QLatin1String("oline"), QStringLiteral("‾"));
    sEntities.insert(QLatin1String("omega"), QStringLiteral("ω"));
    sEntities.insert(QLatin1String("omicron"), QStringLiteral("ο"));
    sEntities.insert(QLatin1String("oplus"), QStringLiteral("⊕"));
    sEntities.insert(QLatin1String("or"), QStringLiteral("∨"));
    sEntities.insert(QLatin1String("ordf"), QStringLiteral("ª"));
    sEntities.insert(QLatin1String("ordm"), QStringLiteral("º"));
    sEntities.insert(QLatin1String("oslash"), QStringLiteral("ø"));
    sEntities.insert(QLatin1String("otilde"), QStringLiteral("õ"));
    sEntities.insert(QLatin1String("otimes"), QStringLiteral("⊗"));
    sEntities.insert(QLatin1String("ouml"), QStringLiteral("ö"));
    sEntities.insert(QLatin1String("para"), QStringLiteral("¶"));
    sEntities.insert(QLatin1String("part"), QStringLiteral("∂"));
    sEntities.insert(QLatin1String("permil"), QStringLiteral("‰"));
    sEntities.insert(QLatin1String("perp"), QStringLiteral("⊥"));
    sEntities.insert(QLatin1String("phi"), QStringLiteral("φ"));
    sEntities.insert(QLatin1String("pi"), QStringLiteral("π"));
    sEntities.insert(QLatin1String("piv"), QStringLiteral("ϖ"));
    sEntities.insert(QLatin1String("plusmn"), QStringLiteral("±"));
    sEntities.insert(QLatin1String("pound"), QStringLiteral("£"));
    sEntities.insert(QLatin1String("prime"), QStringLiteral("′"));
    sEntities.insert(QLatin1String("prod"), QStringLiteral("∏"));
    sEntities.insert(QLatin1String("prop"), QStringLiteral("∝"));
    sEntities.insert(QLatin1String("psi"), QStringLiteral("ψ"));
    sEntities.insert(QLatin1String("quot"), QStringLiteral("\""));
    sEntities.insert(QLatin1String("rArr"), QStringLiteral("⇒"));
    sEntities.insert(QLatin1String("radic"), QStringLiteral("√"));
    sEntities.insert(QLatin1String("rang"), QStringLiteral("〉"));
    sEntities.insert(QLatin1String("raquo"), QStringLiteral("»"));
    sEntities.insert(QLatin1String("rarr"), QStringLiteral("→"));
    sEntities.insert(QLatin1String("rceil"), QStringLiteral("⌉"));
    sEntities.insert(QLatin1String("rdquo"), QStringLiteral("”"));
    sEntities.insert(QLatin1String("real"), QStringLiteral("ℜ"));
    sEntities.insert(QLatin1String("reg"), QStringLiteral("®"));
    sEntities.insert(QLatin1String("rfloor"), QStringLiteral("⌋"));
    sEntities.insert(QLatin1String("rho"), QStringLiteral("ρ"));
    sEntities.insert(QLatin1String("rlm"), QStringLiteral("\xE2\x80\x8F"));
    sEntities.insert(QLatin1String("rsaquo"), QStringLiteral("›"));
    sEntities.insert(QLatin1String("rsquo"), QStringLiteral("’"));
    sEntities.insert(QLatin1String("sbquo"), QStringLiteral("‚"));
    sEntities.insert(QLatin1String("scaron"), QStringLiteral("š"));
    sEntities.insert(QLatin1String("sdot"), QStringLiteral("⋅"));
    sEntities.insert(QLatin1String("sect"), QStringLiteral("§"));
    sEntities.insert(QLatin1String("shy"), QStringLiteral("\xC2\xAD"));
    sEntities.insert(QLatin1String("sigma"), QStringLiteral("σ"));
    sEntities.insert(QLatin1String("sigmaf"), QStringLiteral("ς"));
    sEntities.insert(QLatin1String("sim"), QStringLiteral("∼"));
    sEntities.insert(QLatin1String("spades"), QStringLiteral("♠"));
    sEntities.insert(QLatin1String("sub"), QStringLiteral("⊂"));
    sEntities.insert(QLatin1String("sube"), QStringLiteral("⊆"));
    sEntities.insert(QLatin1String("sum"), QStringLiteral("∑"));
    sEntities.insert(QLatin1String("sup1"), QStringLiteral("¹"));
    sEntities.insert(QLatin1String("sup2"), QStringLiteral("²"));
    sEntities.insert(QLatin1String("sup3"), QStringLiteral("³"));
    sEntities.insert(QLatin1String("sup"), QStringLiteral("⊃"));
    sEntities.insert(QLatin1String("supe"), QStringLiteral("⊇"));
    sEntities.insert(QLatin1String("szlig"), QStringLiteral("ß"));
    sEntities.insert(QLatin1String("tau"), QStringLiteral("τ"));
    sEntities.insert(QLatin1String("there4"), QStringLiteral("∴"));
    sEntities.insert(QLatin1String("theta"), QStringLiteral("θ"));
    sEntities.insert(QLatin1String("thetasym"), QStringLiteral("ϑ"));
    sEntities.insert(QLatin1String("thinsp"), QStringLiteral(" "));
    sEntities.insert(QLatin1String("thorn"), QStringLiteral("þ"));
    sEntities.insert(QLatin1String("tilde"), QStringLiteral("˜"));
    sEntities.insert(QLatin1String("times"), QStringLiteral("×"));
    sEntities.insert(QLatin1String("trade"), QStringLiteral("™"));
    sEntities.insert(QLatin1String("uArr"), QStringLiteral("⇑"));
    sEntities.insert(QLatin1String("uacute"), QStringLiteral("ú"));
    sEntities.insert(QLatin1String("uarr"), QStringLiteral("↑"));
    sEntities.insert(QLatin1String("ucirc"), QStringLiteral("û"));
    sEntities.insert(QLatin1String("ugrave"), QStringLiteral("ù"));
    sEntities.insert(QLatin1String("uml"), QStringLiteral("¨"));
    sEntities.insert(QLatin1String("upsih"), QStringLiteral("ϒ"));
    sEntities.insert(QLatin1String("upsilon"), QStringLiteral("υ"));
    sEntities.insert(QLatin1String("uuml"), QStringLiteral("ü"));
    sEntities.insert(QLatin1String("weierp"), QStringLiteral("℘"));
    sEntities.insert(QLatin1String("xi"), QStringLiteral("ξ"));
    sEntities.insert(QLatin1String("yacute"), QStringLiteral("ý"));
    sEntities.insert(QLatin1String("yen"), QStringLiteral("¥"));
    sEntities.insert(QLatin1String("yuml"), QStringLiteral("ÿ"));
    sEntities.insert(QLatin1String("zeta"), QStringLiteral("ζ"));
    sEntities.insert(QLatin1String("zwj"), QStringLiteral("\xE2\x80\x8D"));
    sEntities.insert(QLatin1String("zwnj"), QStringLiteral("\xE2\x80\x8C"));
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
