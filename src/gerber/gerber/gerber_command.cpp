#include "gerber_command.h"


RenderCommand::RenderCommand(RenderCommand::GerberCommand cmd) {
	command_ = cmd;

	X = Y = W = H = A = 0.0;
	End.X = End.Y = 0.0;
}

