#ifdef INTERFACE
CLASS(NexuizScreenshotViewerDialog) EXTENDS(NexuizDialog)
	METHOD(NexuizScreenshotViewerDialog, fill, void(entity))
	METHOD(NexuizScreenshotViewerDialog, keyDown, float(entity, float, float, float))
	METHOD(NexuizScreenshotViewerDialog, loadScreenshot, void(entity, string))
	METHOD(NexuizScreenshotViewerDialog, close, void(entity))
	ATTRIB(NexuizScreenshotViewerDialog, title, string, "Screenshot Viewer")
	ATTRIB(NexuizScreenshotViewerDialog, name, string, "ScreenshotViewer")
	ATTRIB(NexuizScreenshotViewerDialog, intendedWidth, float, 1)
	ATTRIB(NexuizScreenshotViewerDialog, rows, float, 25)
	ATTRIB(NexuizScreenshotViewerDialog, columns, float, 4)
	ATTRIB(NexuizScreenshotViewerDialog, color, vector, SKINCOLOR_DIALOG_SCREENSHOTVIEWER)
	ATTRIB(NexuizScreenshotViewerDialog, scrList, entity, NULL)
	ATTRIB(NexuizScreenshotViewerDialog, screenshotImage, entity, NULL)
	ATTRIB(NexuizScreenshotViewerDialog, slideShowButton, entity, NULL)
	ATTRIB(NexuizScreenshotViewerDialog, currentScrPath, string, string_null)
ENDCLASS(NexuizScreenshotViewerDialog)
#endif

#ifdef IMPLEMENTATION
void loadScreenshotNexuizScreenshotViewerDialog(entity me, string scrImage)
{
	if (me.currentScrPath == scrImage)
		return;
	if (me.currentScrPath)
		strunzone(me.currentScrPath);
	me.currentScrPath = strzone(scrImage);
	me.screenshotImage.configureNexuizScreenshotImage(me.screenshotImage, me.currentScrPath);
}
void prevScreenshot_Click(entity btn, entity me)
{
	me.scrList.goScreenshot(me.scrList, -1);
}
void nextScreenshot_Click(entity btn, entity me)
{
	me.scrList.goScreenshot(me.scrList, +1);
}
void increaseZoom_Click(entity btn, entity me)
{
	me.screenshotImage.setZoom(me.screenshotImage, -2, FALSE);
}
void decreaseZoom_Click(entity btn, entity me)
{
	me.screenshotImage.setZoom(me.screenshotImage, -1/2, FALSE);
}
void resetZoom_Click(entity btn, entity me)
{
	me.screenshotImage.setZoom(me.screenshotImage, 0, FALSE);
}
void toggleSlideShow_Click(entity btn, entity me)
{
	if (me.slideShowButton.forcePressed)
	{
		me.scrList.stopSlideShow(me.scrList);
		me.slideShowButton.forcePressed = 0;
	}
	else
	{
		me.scrList.startSlideShow(me.scrList);
		me.slideShowButton.forcePressed = 1;
	}
}
float keyDownNexuizScreenshotViewerDialog(entity me, float key, float ascii, float shift)
{
	switch(key)
	{
		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			me.scrList.goScreenshot(me.scrList, -1);
			return 1;
		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			me.scrList.goScreenshot(me.scrList, +1);
			return 1;
		case K_KP_ENTER:
		case K_ENTER:
		case K_SPACE:
			// we cannot use SPACE/ENTER directly, as in a dialog they are needed
			// to press buttons while browsing with only the keyboard
			if (shift & S_CTRL)
			{
				toggleSlideShow_Click(world, me);
				return 1;
			}
			return keyDownListBox(me, key, ascii, shift);
				return 1;
		default:
			if (key == K_MWHEELUP || ascii == '+')
			{
				me.screenshotImage.setZoom(me.screenshotImage, -2, (key == K_MWHEELUP));
				return 1;
			}
			else if (key == K_MWHEELDOWN || ascii == '-')
			{
				me.screenshotImage.setZoom(me.screenshotImage, -1/2, (key == K_MWHEELDOWN));
				return 1;
			}
			if (me.scrList.keyDown(me.scrList, key, ascii, shift))
			{
				// keyDown has already changed the selected item
				me.scrList.goScreenshot(me.scrList, 0);
				return 1;
			}
		else
			return keyDownListBox(me, key, ascii, shift);
		return 1;	
	}
}
void closeNexuizScreenshotViewerDialog(entity me)
{
	me.scrList.stopSlideShow(me.scrList);
	me.slideShowButton.forcePressed = 0;
	closeDialog(me);
}
void fillNexuizScreenshotViewerDialog(entity me)
{
	entity e;
	me.TR(me);
		me.TD(me, me.rows - 1, me.columns, e = makeNexuizScreenshotImage());
			me.screenshotImage = e;
	me.gotoRC(me, me.rows - 1, 0);
		me.TDempty(me, 1/20 * me.columns);
		me.TD(me, 1, 1/20 * me.columns, e = makeNexuizButton("-", '0 0 0'));
			e.onClick = decreaseZoom_Click;
			e.onClickEntity = me;
		me.TD(me, 1, 1/20 * me.columns, e = makeNexuizButton("+", '0 0 0'));
			e.onClick = increaseZoom_Click;
			e.onClickEntity = me;
		me.TD(me, 1, 2/20 * me.columns, e = makeNexuizButton("reset", '0 0 0'));
			e.onClick = resetZoom_Click;
			e.onClickEntity = me;

		me.TDempty(me, 2/20 * me.columns);
		me.TD(me, 1, 3/20 * me.columns, e = makeNexuizButton("Previous", '0 0 0'));
			e.onClick = prevScreenshot_Click;
			e.onClickEntity = me;
		me.TD(me, 1, 3/20 * me.columns, e = makeNexuizButton("Next", '0 0 0'));
			e.onClick = nextScreenshot_Click;
			e.onClickEntity = me;

		me.TDempty(me, 2/20 * me.columns);
		me.TD(me, 1, 4/20 * me.columns, e = makeNexuizButton("Slide show", '0 0 0'));
			e.onClick = toggleSlideShow_Click;
			e.onClickEntity = me;
			me.slideShowButton = e;
}
#endif