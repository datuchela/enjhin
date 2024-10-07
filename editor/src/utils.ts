import { redrawCanvas } from "./canvas.js";
import { refreshButtons } from "./dom.js";
import { type State } from "./types.js";

export function redrawAll(state: State) {
  redrawCanvas(state);
  refreshButtons(state);
}

