//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2020 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#include "workspacefile.h"

#include <algorithm>

#include <QFile>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "log.h"
#include "stringutils.h"

#include "thirdparty/qzip/qzipreader_p.h"
#include "thirdparty/qzip/qzipwriter_p.h"

using namespace mu;
using namespace mu::workspace;

WorkspaceFile::WorkspaceFile(const io::path& filepath)
    : m_filePath(filepath)
{}

QByteArray WorkspaceFile::readRootFile()
{
    QFile file(m_filePath.toQString());
    if (!file.open(QIODevice::ReadOnly)) {
        LOGE() << "failed open file: " << m_filePath;
        return QByteArray();
    }

    QByteArray data = file.readAll();

    QBuffer buf(&data);
    MQZipReader zip(&buf);

    std::string rootFile;
    MetaInf meta;
    if (meta.read(zip)) {
        rootFile = meta.rootFile();
    } else {
        QVector<MQZipReader::FileInfo> fis = zip.fileInfoList();
        if (!fis.isEmpty()) {
            rootFile = fis.first().filePath.toStdString();
        }
    }

    if (rootFile.empty()) {
        LOGE() << "not found root file: " << m_filePath;
        return QByteArray();
    }

    QByteArray fileData = zip.fileData(QString::fromStdString(rootFile));
    if (fileData.isEmpty()) {
        LOGE() << "failed read root file: " << m_filePath;
        return QByteArray();
    }

    return fileData;
}

bool WorkspaceFile::writeRootFile(const std::string& name, const QByteArray& data)
{
    MQZipWriter zip(m_filePath.toQString());

    MetaInf meta;
    meta.setRootFile(name);
    meta.write(zip);

    zip.addFile(QString::fromStdString(name), data);

    bool ret = zip.status() == MQZipWriter::NoError;

    if (!ret) {
        LOGE() << "Error while writing workspace, zip status: " << zip.status();
    }

    return ret;
}

void WorkspaceFile::MetaInf::setRootFile(const std::string& name)
{
    m_rootFile = name;
}

std::string WorkspaceFile::MetaInf::rootFile() const
{
    return m_rootFile;
}

void WorkspaceFile::MetaInf::write(MQZipWriter& zip)
{
    QByteArray data;
    writeContainer(&data);
    zip.addFile("META-INF/container.xml", data);
}

bool WorkspaceFile::MetaInf::read(const MQZipReader& zip)
{
    QByteArray container = zip.fileData("META-INF/container.xml");
    if (container.isEmpty()) {
        LOGE() << "not found META-INF/container.xml";
        return false;
    }

    readContainer(container);
    if (m_rootFile.empty()) {
        return false;
    }

    return true;
}

void WorkspaceFile::MetaInf::readContainer(const QByteArray& data)
{
    QXmlStreamReader xml(data);
    while (xml.readNextStartElement()) {
        if ("container" != xml.name()) {
            xml.skipCurrentElement();
            continue;
        }

        while (xml.readNextStartElement()) {
            if ("rootfiles" != xml.name()) {
                xml.skipCurrentElement();
                continue;
            }

            while (xml.readNextStartElement()) {
                if ("rootfile" != xml.name()) {
                    xml.skipCurrentElement();
                    continue;
                }

                QString path = xml.attributes().value("full-path").toString();
                m_rootFile = path.toStdString();
                return;
            }
        }
    }
}

void WorkspaceFile::MetaInf::writeContainer(QByteArray* data) const
{
    IF_ASSERT_FAILED(data) {
        return;
    }

    QXmlStreamWriter xml(data);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("container");
    xml.writeStartElement("rootfiles");

    xml.writeStartElement("rootfile");
    xml.writeAttribute("full-path", QString::fromStdString(m_rootFile));
    xml.writeEndElement();

    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
}
