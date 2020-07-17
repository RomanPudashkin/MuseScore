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

#ifndef MU_INSPECTORS_SCOREAPPEARANCESETTINGSMODEL_H
#define MU_INSPECTORS_COREAPPEARANCESETTINGSMODEL_H

#include "view/abstractinspectormodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

class PageTypeListModel;

namespace mu {
namespace inspector {
class ScoreAppearanceSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT

    INJECT(inspector, actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(PageTypeListModel* pageTypeListModel READ pageTypeListModel CONSTANT)
    Q_PROPERTY(int orientationType READ orientationType WRITE setOrientationType NOTIFY orientationTypeChanged)
    Q_PROPERTY(qreal staffSpacing READ staffSpacing WRITE setStaffSpacing NOTIFY staffSpacingChanged)
    Q_PROPERTY(qreal staffDistance READ staffDistance WRITE setStaffDistance NOTIFY staffDistanceChanged)

public:
    explicit ScoreAppearanceSettingsModel(QObject* parent, IElementRepositoryService* repository);

    Q_INVOKABLE void showPageSettings();
    Q_INVOKABLE void showStyleSettings();

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    bool hasAcceptableElements() const override;

    PageTypeListModel* pageTypeListModel() const;

    int orientationType() const;
    qreal staffSpacing() const;
    qreal staffDistance() const;

public slots:
    void setPageTypeListModel(PageTypeListModel* pageTypeListModel);
    void setOrientationType(int orientationType);
    void setStaffSpacing(qreal staffSpacing);
    void setStaffDistance(qreal staffDistance);

signals:
    void orientationTypeChanged(int orientationType);
    void staffSpacingChanged(qreal staffSpacing);
    void staffDistanceChanged(qreal staffDistance);

private:
    void updatePageSize();

    int m_orientationType = 0;
    qreal m_staffSpacing = 0.0;
    qreal m_staffDistance = 0.0;

    PageTypeListModel* m_pageTypeListModel = nullptr;
};
}
}

#endif // MU_INSPECTORS_SCOREAPPEARANCESETTINGSMODEL_H
