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

#include <gtest/gtest.h>

#include "userscores/internal/templatesrepository.h"

#include "domain/notation/tests/mocks/msczreadermock.h"
#include "mocks/userscoresconfigurationmock.h"
#include "system/tests/mocks/fsoperationsmock.h"

using ::testing::_;
using ::testing::Return;

using namespace mu;
using namespace mu::domain::notation;
using namespace mu::userscores;
using namespace mu::framework;

class TemplatesRepositoryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_repository = std::make_shared<TemplatesRepository>();
        m_msczReader = std::make_shared<MsczReaderMock>();
        m_fsOperations = std::make_shared<FsOperationsMock>();
        m_configuration = std::make_shared<UserScoresConfigurationMock>();

        m_repository->setconfiguration(m_configuration);
        m_repository->setmsczReader(m_msczReader);
        m_repository->setfsOperations(m_fsOperations);
    }

    Meta createMeta(const QString& title) const
    {
        Meta meta;

        meta.title = title;
        meta.creationDate = QDate::currentDate();

        return meta;
    }

    std::shared_ptr<TemplatesRepository> m_repository;
    std::shared_ptr<UserScoresConfigurationMock> m_configuration;
    std::shared_ptr<MsczReaderMock> m_msczReader;
    std::shared_ptr<FsOperationsMock> m_fsOperations;
};

namespace mu {
namespace userscores {
bool operator==(const Template& templ1, const Template& templ2)
{
    bool equals = true;

    equals &= (templ1.title == templ2.title);
    equals &= (templ1.categoryTitle == templ2.categoryTitle);
    equals &= (templ1.creationDate == templ2.creationDate);

    return equals;
}
}
}

TEST_F(TemplatesRepositoryTest, Templates)
{
    // [GIVEN] All paths to templates dirs
    QStringList templatesDirPaths {
        "/path/to/templates",
        "/path/to/user/templates"
        "/extensions/templates"
    };

    EXPECT_CALL(*m_configuration, templatesDirPaths())
            .WillOnce(Return(templatesDirPaths));

    // [GIVEN] All paths to mscz files
    QStringList allPathsToMsczFiles;

    for (int i = 0; i < templatesDirPaths.size(); ++i) {
        QString dirPath = templatesDirPaths[i];
        QString filePath = dirPath + QString("/file%1.mscz").arg(i);
        allPathsToMsczFiles << filePath;

        QStringList filters = { "*.mscz", "*.mscx" };

        RetVal<QStringList> result = RetVal<QStringList>::make_ok(QStringList { filePath });
        ON_CALL(*m_fsOperations, scanFiles(dirPath, filters, IFsOperations::ScanMode::IncludeSubdirs))
                .WillByDefault(Return(result));
    }

    // [GIVEN] Templates meta
    Templates expectedTemplates;

    for (const QString& path: allPathsToMsczFiles) {
        Meta meta = createMeta(path);

        ON_CALL(*m_msczReader, readMeta(io::pathFromQString(path)))
                .WillByDefault(Return(RetVal<Meta>::make_ok(meta)));

        Template templ(meta);
        expectedTemplates << templ;
    }

    // [WHEN] Get templates meta
    RetVal<Templates> templates = m_repository->templates();

    // [THEN] Successfully got templates meta
    EXPECT_TRUE(templates.ret);

    EXPECT_EQ(templates.val.size(), expectedTemplates.size());
    for (const Template& templ: templates.val) {
        EXPECT_TRUE(expectedTemplates.contains(templ));
    }
}
