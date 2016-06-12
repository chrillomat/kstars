/** *************************************************************************
                          skymaplite.h  -  K Desktop Planetarium
                             -------------------
    begin                : 30/04/2016
    copyright            : (C) 2016 by Artem Fedoskin
    email                : afedoskin3@gmail.com
 ***************************************************************************/
/** *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SKYMAPLITE_H_
#define SKYMAPLITE_H_

#include "skyobjects/skypoint.h"
#include "skyobjects/skyline.h"

#include <QTimer>

#include <config-kstars.h>
#include <QQuickItem>
#include <QPolygonF>
#include "kstarsdata.h"

class dms;
class KStarsData;
class SkyObject;
class Projector;
class SolarSystemSingleComponent;
class PlanetsItem;
class AsteroidsItem;
class CometsItem;
class PlanetMoonsComponent;
class HorizonItem;
class LinesItem;

class QSGTexture;

/** @class SkyMapLite
        *
        *This is the main item that displays all SkyItems. After its instantiation it is reparanted
        *to an object with objectName SkyMapLiteWrapper in main.qml. To display SkyItems they are reparanted
        *to instance of SkyMapLite.
        *
        *SkyMapLite handles most user interaction events (both mouse and keyboard).
        *
        *@short Item for displaying sky objects; also handles user interaction events.
        *@author Artem Fedoskin
        *@version 1.0
        */

class SkyMapLite : public QQuickItem {

    Q_OBJECT

protected:
    /**
    *Constructor.
    */
    explicit SkyMapLite(QQuickItem* parent = 0);

    virtual QSGNode* updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData);

public:
    static SkyMapLite* createInstance(QQuickItem* parent = 0);

    static SkyMapLite* Instance() { return pinstance; }

    /*enum Projection { Lambert,
                      AzimuthalEquidistant,
                      Orthographic,
                      Equirectangular,
                      Stereographic,
                      Gnomonic,
                      UnknownProjection };*/

    static bool IsSlewing() { return pinstance->isSlewing(); }

    /** Destructor (empty) */
    ~SkyMapLite();

    /*enum Projection { Lambert,
                      AzimuthalEquidistant,
                      Orthographic,
                      Equirectangular,
                      Stereographic,
                      Gnomonic,
                      UnknownProjection };*/

    /** @short Retrieve the Focus point; the position on the sky at the
        *center of the skymap.
        *@return a pointer to the central focus point of the sky map
        */

    /** @return the angular field of view of the sky map, in degrees.
    *@note it must use either the height or the width of the window to calculate the
    *FOV angle.  It chooses whichever is larger.
    */
    //float fov();

    /** @short Update the focus position according to current options. */
    //void updateFocus();

    /** @short Retrieve the Focus point; the position on the sky at the
        *center of the skymap.
        *@return a pointer to the central focus point of the sky map
        */
    SkyPoint* focus() { return &Focus; }

    /** @short retrieve the Destination position.
        *
        *The Destination is the point on the sky to which the focus will
        *be moved.
        *
        *@return a pointer to the destination point of the sky map
        */
    SkyPoint* destination() { return &Destination; }

    /** @short retrieve the FocusPoint position.
        *
        *The FocusPoint stores the position on the sky that is to be
        *focused next.  This is not exactly the same as the Destination
        *point, because when the Destination is set, it will begin slewing
        *immediately.
        *
        *@return a pointer to the sky point which is to be focused next.
        */
    SkyPoint* focusPoint() { return &FocusPoint; }

    /** @short sets the central focus point of the sky map.
        *@param f a pointer to the SkyPoint the map should be centered on
        */
    void setFocus( SkyPoint *f );

    /** @short sets the focus point of the skymap, using ra/dec coordinates
        *
        *@note This function behaves essentially like the above function.
        *It differs only in the data types of its arguments.
        *
        *@param ra the new right ascension
        *@param dec the new declination
        */
    void setFocus( const dms &ra, const dms &dec );

    /** @short sets the focus point of the sky map, using its alt/az coordinates
        *@param alt the new altitude
        *@param az the new azimuth
        */
    void setFocusAltAz( const dms &alt, const dms & az);

    /** @short sets the destination point of the sky map.
        *@note setDestination() emits the destinationChanged() SIGNAL,
        *which triggers the SLOT function SkyMap::slewFocus().  This
        *function iteratively steps the Focus point toward Destination,
        *repainting the sky at each step (if Options::useAnimatedSlewing()==true).
        *@param f a pointer to the SkyPoint the map should slew to
        */
    void setDestination( const SkyPoint& f );

    /** @short sets the destination point of the skymap, using ra/dec coordinates.
        *
        *@note This function behaves essentially like the above function.
        *It differs only in the data types of its arguments.
        *
        *@param ra the new right ascension
        *@param dec the new declination
        */
    void setDestination( const dms &ra, const dms &dec );

    /** @short sets the destination point of the sky map, using its alt/az coordinates.
        *@param alt the new altitude
        *@param az the new azimuth
        */
    void setDestinationAltAz( const dms &alt, const dms & az);

    /** @short set the FocusPoint; the position that is to be the next Destination.
        *@param f a pointer to the FocusPoint SkyPoint.
        */
    void setFocusPoint( SkyPoint *f ) { if ( f ) FocusPoint = *f; }

    /** @short Retrieve the ClickedPoint position.
        *
        *When the user clicks on a point in the sky map, the sky coordinates of the mouse
        *cursor are stored in the private member ClickedPoint.  This function retrieves
        *a pointer to ClickedPoint.
        *@return a pointer to ClickedPoint, the sky coordinates where the user clicked.
        */
    SkyPoint* clickedPoint() { return &ClickedPoint; }

    /** @short Set the ClickedPoint to the skypoint given as an argument.
        *@param f pointer to the new ClickedPoint.
        */
    void setClickedPoint( SkyPoint *f );

    /** @short Retrieve the object nearest to a mouse click event.
        *
        *If the user clicks on the sky map, a pointer to the nearest SkyObject is stored in
        *the private member ClickedObject.  This function returns the ClickedObject pointer,
        *or NULL if there is no CLickedObject.
        *@return a pointer to the object nearest to a user mouse click.
        */
    SkyObject* clickedObject() const { return ClickedObject; }

    /** @short Set the ClickedObject pointer to the argument.
        *@param o pointer to the SkyObject to be assigned as the ClickedObject
        */
    void setClickedObject( SkyObject *o );

    /** @short Retrieve the object which is centered in the sky map.
        *
        *If the user centers the sky map on an object (by double-clicking or using the
        *Find Object dialog), a pointer to the "focused" object is stored in
        *the private member FocusObject.  This function returns a pointer to the
        *FocusObject, or NULL if there is not FocusObject.
        *@return a pointer to the object at the center of the sky map.
        */
    SkyObject* focusObject() const { return FocusObject; }

    /** @short Set the FocusObject pointer to the argument.
        *@param o pointer to the SkyObject to be assigned as the FocusObject
        */
    void setFocusObject( SkyObject *o );

    /** @ Set zoom factor.
      *@param factor zoom factor
      */
    void setZoomFactor(double factor);

    /** @short Call to set up the projector before update of SkyItems positions begins. */
    void setupProjector();

    /** @short Returns index for a Harvard spectral classification */
    int harvardToIndex(char c);

    /** @short returns cache of star images
     *  @return star images cache
     */
    QVector<QVector<QPixmap*>> getImageCache();

    QSGTexture *textToTexture(QString text, QColor color = QColor(255,255,255), QFont font = QFont());

    /**
     * @short returns cached texture from textureCache.
     *
     * Use outside of scene graph rendering thread (e.g. not during call to updatePaintNode)
     * is prohibited!
     * @param size size of the star
     * @param spType spectral class
     * @return cached QSGTexture from textureCache
     */
    QSGTexture* getCachedTexture(int size, char spType);

    /** Called when SkyMapComposite finished loading all SkyComponents */
    inline void loadingFinished() { m_loadingFinished = true; }

    bool isSlewing() const;

    // NOTE: This method is draw-backend independent.
    /** @short update the geometry of the angle ruler. */
    //void updateAngleRuler();

    /*@*@short Convenience function for shutting off tracking mode.  Just calls KStars::slotTrack().
        */
    //void stopTracking();

    /** Get the current projector.
        @return a pointer to the current projector. */
    inline const Projector * projector() const { return m_proj; }

    /**
     * @return font of labels
     */
    inline QFont skyFont() const { return m_skyFont; }

    /**
     *@short Proxy method for SkyMapDrawAbstract::drawObjectLabels()
     */
    //inline void drawObjectLabels( QList< SkyObject* >& labelObjects ) { dynamic_cast<SkyMapDrawAbstract *>(m_SkyMapDraw)->drawObjectLabels( labelObjects ); }

    /*void setPreviewLegend(bool preview) { m_previewLegend = preview; }

    void setLegend(const Legend &legend) { m_legend = legend; }

    bool isInObjectPointingMode() const { return m_objPointingMode; }

    void setObjectPointingMode(bool enabled) { m_objPointingMode = enabled; }

    void setFovCaptureMode(bool enabled) { m_fovCaptureMode = enabled; }

    bool isInFovCaptureMode() const { return m_fovCaptureMode; }

    SkyPoint getCenterPoint();*/

public slots:
     /** Called whenever wrappers' width or height are changed. Probably will be used to
     * update positions of items.
     */
    void resizeItem();

    /** Recalculates the positions of objects in the sky, and then repaints the sky map.
     * If the positions don't need to be recalculated, use update() instead of forceUpdate().
     * This saves a lot of CPU time.
     */
    void forceUpdate();

    /** @short Left for compatibility reasons
     * @see forceUpdate()
     */
    void forceUpdateNow() { forceUpdate(); }

    /**
     * @short Update the focus point and call forceUpdate()
     * @param now is saved for compatibility reasons
     */
    void slotUpdateSky( bool now ) {Q_UNUSED(now)}

    /** Toggle visibility of geo infobox */
    //void slotToggleGeoBox(bool);

    /** Toggle visibility of focus infobox */
    //void slotToggleFocusBox(bool);

    /** Toggle visibility of time infobox */
    //void slotToggleTimeBox(bool);

    /** Toggle visibility of all infoboxes */
    //void slotToggleInfoboxes(bool);

    /** Step the Focus point toward the Destination point.  Do this iteratively, redrawing the Sky
     * Map after each step, until the Focus point is within 1 step of the Destination point.
     * For the final step, snap directly to Destination, and redraw the map.
     */
    void slewFocus();

    /** @short Center the display at the point ClickedPoint.
     *
     * The essential part of the function is to simply set the Destination point, which will emit
     * the destinationChanged() SIGNAL, which triggers the slewFocus() SLOT.  Additionally, this
     * function performs some bookkeeping tasks, such updating whether we are tracking the new
     * object/position, adding a Planet Trail if required, etc.
     *
     * @see destinationChanged()
     * @see slewFocus()
     */
    //void slotCenter();

    /** @short Popup menu function: Display 1st-Generation DSS image with the Image Viewer.
     * @note the URL is generated using the coordinates of ClickedPoint.
     */
    //void slotDSS();

    /** @short Popup menu function: Display Sloan Digital Sky Survey image with the Image Viewer.
     * @note the URL is generated using the coordinates of ClickedPoint.
     */
    //void slotSDSS();

    /** @short Popup menu function: Show webpage about ClickedObject
     * (only available for some objects).
     */
    //void slotInfo();

    /** @short Popup menu function: Show image of ClickedObject
     * (only available for some objects).
     */
    //void slotImage();

    /** @short Popup menu function: Show the Detailed Information window for ClickedObject. */
    //void slotDetail();

    /** Checks whether the timestep exceeds a threshold value.  If so, sets
     * ClockSlewing=true and sets the SimClock to ManualMode.
     */
    void slotClockSlewing();

  //  void slotBeginStarHop(); // TODO: Add docs

    /** Render eyepiece view */
    //void slotEyepieceView();

    /** Zoom in one step. */
    void slotZoomIn();

    /** Zoom out one step. */
    void slotZoomOut();

    /** Set default zoom. */
    void slotZoomDefault();

    /** Object pointing for Printing Wizard done */
    //void slotObjectSelected();

    //void slotCancelLegendPreviewMode();

signals:
    /** Emitted by setDestination(), and connected to slewFocus().  Whenever the Destination
     * point is changed, slewFocus() will iteratively step the Focus toward Destination
     * until it is reached.
     * @see SkyMap::setDestination()
     * @see SkyMap::slewFocus()
     */
    void destinationChanged();

    /** Emitted when zoom level is changed. */
    void zoomChanged();

    /** Emitted when current object changed. */
    void objectChanged(SkyObject*);

    /** Emitted when pointing changed. (At least should) */
    void positionChanged(SkyPoint*);

    /** Emitted when position under mouse changed. */
    void mousePointChanged(SkyPoint*);

    /** Emitted when a position is clicked */
    void positionClicked(SkyPoint*);

protected:
    /** Process keystrokes:
     * @li arrow keys  Slew the map
     * @li +/- keys  Zoom in and out
     * @li <i>Space</i>  Toggle between Horizontal and Equatorial coordinate systems
     * @li 0-9  Go to a major Solar System body (0=Sun; 1-9 are the major planets, except 3=Moon)
     * @li [  Place starting point for measuring an angular distance
     * @li ]  End point for Angular Distance; display measurement.
     * @li <i>Escape</i>  Cancel Angular measurement
     * @li ,/<  Step backward one time step
     * @li ./>  Step forward one time step
     */
    //virtual void keyPressEvent( QKeyEvent *e );

    /** When keyRelease is triggered, just set the "slewing" flag to false,
     * and update the display (to draw objects that are hidden when slewing==true). */
    //virtual void keyReleaseEvent( QKeyEvent *e );

    /** Determine RA, Dec coordinates of clicked location.  Find the SkyObject
     * which is nearest to the clicked location.
     *
     * If left-clicked: Set set mouseButtonDown==true, slewing==true; display
     * nearest object name in status bar.
     * If right-clicked: display popup menu appropriate for nearest object.
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /** set mouseButtonDown==false, slewing==false */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /** Center SkyMap at double-clicked location  */
    virtual void mouseDoubleClickEvent( QMouseEvent *e );

    /** This function does several different things depending on the state of the program:
     * @li If Angle-measurement mode is active, update the end-ruler point to the mouse cursor,
     *     and continue this function.
     * @li If we are defining a ZoomBox, update the ZoomBox rectangle, redraw the screen,
     *     and return.
     * @li If dragging the mouse in the map, update focus such that RA, Dec under the mouse
     *     cursor remains constant.
     * @li If just moving the mouse, simply update the curso coordinates in the status bar.
     */
    virtual void mouseMoveEvent( QMouseEvent *e );

    /** Zoom in and out with the mouse wheel. */
    virtual void wheelEvent( QWheelEvent *e );

    /**
     * @short this function handles zooming in and out using "pinch to zoom" gesture
     */
    virtual void touchEvent( QTouchEvent *e);

    /** If the skymap will be resized, the sky must be new computed. So this
     * function calls explicitly new computing of the skymap.
     */
    //virtual void resizeEvent( QResizeEvent * );

private slots:
    /** @short display tooltip for object under cursor. It's called by m_HoverTimer.
     *  if mouse didn't moved for last HOVER_INTERVAL milliseconds.
     */
    //void slotTransientLabel();

    /** Set the shape of mouse cursor to a cross with 4 arrows. */
    void setMouseMoveCursor();

private:

    /** @short Sets the shape of the default mouse cursor to a cross. */
    void setDefaultMouseCursor();

    /** @short Sets the shape of the mouse cursor to a magnifying glass. */
    void setZoomMouseCursor();

    /** Calculate the zoom factor for the given keyboard modifier
     */
    double zoomFactor( const int modifier );

    /** calculate the magnitude factor (1, .5, .2, or .1) for the given
     * keyboard modifier.
     */
    double magFactor( const int modifier );

    /** Decrease the magnitude limit by a step size determined by the
     * keyboard modifier.
     * @param modifier
     */
    void decMagLimit( const int modifier );

     /** Increase the magnitude limit by a step size determined by the
     * keyboard modifier.
     * @param modifier
     */
    void incMagLimit( const int modifier );

    /** Convenience routine to either zoom in or increase mag limit
     * depending on the Alt modifier.  The Shift and Control modiifers
     * will adjust the size of the zoom or the mag step.
     * @param modifier
     */
    void zoomInOrMagStep( const int modifier );

    /** Convenience routine to either zoom out or decraase mag limit
     * depending on the Alt modifier.  The Shift and Control modiifers
     * will adjust the size of the zoom or the mag step.
     * @param modifier
     */
    void zoomOutOrMagStep( const int modifier );

    /** @short Initializes images of Stars and puts them in cache (copied from SkyQPainter)*/
    void initStarImages();

    bool mouseButtonDown, midMouseButtonDown;
    // true if mouseMoveEvent; needed by setMouseMoveCursor
    bool mouseMoveCursor;
    bool slewing, clockSlewing;
    // true if pinch to zoom or pinch to rotate is performed
    bool pinch;
    //if false only old pixmap will repainted with bitBlt(), this
    // saves a lot of cpu usage
    bool computeSkymap;
    // True if we are either looking for angular distance or star hopping directions
    //bool rulerMode;
    // True only if we are looking for star hopping directions. If
    // false while rulerMode is true, it means we are measuring angular
    // distance. FIXME: Find a better way to do this
    //bool starHopDefineMode;
    //double y0;

    //double m_Scale;
    int count;

    KStarsData *data;

    //True if SkyMapComposite has finished loading of SkyComponents
    bool m_loadingFinished;

    /** @short Coordinates of point under cursor. It's update in
     * function mouseMoveEvent
     */
    SkyPoint m_MousePoint;

    SkyPoint  Focus, ClickedPoint, FocusPoint, Destination;
    SkyObject *ClickedObject, *FocusObject;

    //SkyLine AngularRuler; //The line for measuring angles in the map
    QRect ZoomRect; //The manual-focus circle.

    // Mouse should not move for that interval to display tooltip
    static const int HOVER_INTERVAL = 500;
    // Timer for tooltips
    QTimer m_HoverTimer;

    bool m_objPointingMode;
    bool m_fovCaptureMode;

    Projector* m_proj;

    static SkyMapLite *pinstance;
    QQuickItem *m_SkyMapLiteWrapper;

    QFont m_skyFont;

    static int starColorMode;

    const SkyPoint *m_rulerStartPoint; // Good to keep the original ruler start-point for purposes of dynamic_cast

    // This can be later changed
    // Total number of sizes of stars.
    const int nStarSizes;
    // Total number of specatral classes
    // N.B. Must be in sync with harvardToIndex
    const int nSPclasses;

    //This can be later changed
    // Cache for star images.
    QVector<QVector<QPixmap*>> imageCache;
    //Textures created from cached star images
    QVector<QVector<QSGTexture*>> textureCache;

};

#endif