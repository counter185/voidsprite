#include "Brush1pxLinePathfind.h"
#include "../UtilPathfind.h"
#include "../background_operation.h"

void Brush1pxLinePathfind::lineSelected(MainEditor* editor, XY from, XY to)
{
	g_startNewOperation([this, editor, from, to](OperationProgressReport* progressReport) {
		progressReport->enterSection("Pathfinding...");
		std::vector<Node> pathfindResult = genAStar(editor->getCurrentLayer(), from, to, progressReport);
		for (Node& n : pathfindResult) {
			editor->SetPixel(XY{ n.x, n.y }, editor->getActiveColor());
		}
	});
}
