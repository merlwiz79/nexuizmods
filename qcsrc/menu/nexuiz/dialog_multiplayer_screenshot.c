#ifdef INTERFACE
CLASS(NexuizScreenshotBrowserTab) EXTENDS(NexuizTab)
	METHOD(NexuizScreenshotBrowserTab, fill, void(entity))
	ATTRIB(NexuizScreenshotBrowserTab, title, string, "Screenshot")
	ATTRIB(NexuizScreenshotBrowserTab, intendedWidth, float, 1)
	ATTRIB(NexuizScreenshotBrowserTab, rows, float, 22)
	ATTRIB(NexuizScreenshotBrowserTab, columns, float, 6.5)
	ATTRIB(NexuizScreenshotBrowserTab, name, string, "ScreenshotBrowser")
	
	METHOD(NexuizScreenshotBrowserTab, loadPreviewScreenshot, void(entity, string))
	ATTRIB(NexuizScreenshotBrowserTab, screenshotImage, entity, NULL)
	ATTRIB(NexuizScreenshotBrowserTab, currentScrPath, string, string_null)
ENDCLASS(NexuizScreenshotBrowserTab)
entity makeNexuizScreenshotBrowserTab();
#endif

#ifdef IMPLEMENTATION
entity makeNexuizScreenshotBrowserTab()
{
	entity me;
	me = spawnNexuizScreenshotBrowserTab();
	me.configureDialog(me);
	return me;
}
void loadPreviewScreenshotNexuizScreenshotBrowserTab(entity me, string scrImage)
{
	if (me.currentScrPath == scrImage)
		return;
	if (me.currentScrPath)
		strunzone(me.currentScrPath);
	me.currentScrPath = strzone(scrImage);
	me.screenshotImage.configureNexuizScreenshotImage(me.screenshotImage, me.currentScrPath);
}
void fillNexuizScreenshotBrowserTab(entity me)
{
	entity e, btn, slist;
	slist = makeNexuizScreenshotList();
	const float slist_height = me.rows - 2;
	me.TR(me);
		me.TD(me, 1, 0.5, e = makeNexuizTextLabel(0, "Filter:"));
		me.TD(me, 1, 0.5, btn = makeNexuizButton("Clear", '0 0 0'));
			btn.onClick = InputBox_Clear_Click;
		me.TD(me, 1, me.columns - 1.5, e = makeNexuizInputBox(0, string_null));
			e.onChange = ScreenshotList_Filter_Would_Change;
			e.onChangeEntity = slist;
			btn.onClickEntity = e;
			slist.screenshotViewerDialog = main.screenshotViewerDialog;
			main.screenshotViewerDialog.scrList = slist;
		me.TD(me, 1, 0.5, e = makeNexuizButton("Refresh", '0 0 0'));
			e.onClick = ScreenshotList_Refresh_Click;
			e.onClickEntity = slist;
	me.TR(me);
		me.TD(me, slist_height, me.columns, slist);

	me.gotoRC(me, slist_height + 1, 0);
		me.TD(me, 1, me.columns, e = makeNexuizButton("Open in the viewer", '0 0 0'));
			e.onClick = StartScreenshot_Click;
			e.onClickEntity = slist;
/*
	me.TR(me);
		me.TD(me, me.rows - me.currentRow, me.columns, e = makeNexuizScreenshotImage());
			e.showTitle = 0;
			me.screenshotImage = e;
			slist.screenshotPreview = e;
			slist.screenshotBrowserDialog = me;
*/
}
#endif
