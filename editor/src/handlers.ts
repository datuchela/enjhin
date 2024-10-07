import { refreshButtons, refreshListView } from "./dom.js";
import { addBody, addParticle, setCurrentBodyId, setMode } from "./state.js";
import { redrawAll } from "./utils.js";
import { Mode, type SoftBody, type State, type Vector2 } from "./types.js";

export function handleNewBodyClick(state: State) {
  const newBody: SoftBody = {
    id: `${++state.idDispatcher.bodyIdCounter}`,
    springs: [],
    particles: [],
  };
  addBody(newBody, state);
  setCurrentBodyId(newBody.id, state);
  refreshListView(state);
}

export function handleModeClick(mode: Mode, state: State) {
  setMode(mode, state);
  refreshButtons(state);
}

export function handleExportClick(e: MouseEvent, state: State) {
  e.preventDefault();
  console.log(state.data);
}

export function handleMouseClick(e: MouseEvent, state: State) {
  const rect = state.element.canvas.getBoundingClientRect();
  const position: Vector2 = { x: e.clientX - rect.left, y: e.clientY - rect.top };
  if (position.x < 0) {
    position.x = 0;
  }
  if (position.y < 0) {
    position.y = 0;
  }

  switch (state.mode) {
    case Mode.MOVE:
      console.log("MOVE mode");
      break;
    case Mode.SELECT:
      console.log("SELECT mode");
      break;
    case Mode.INSERT:
      if (state.currentBodyId === null) break;
      console.log("INSERT mode");
      addParticle(
        {
          id: `${++state.idDispatcher.particleIdCounter}`,
          position,
        },
        state.currentBodyId,
        state
      );
      break;
    case Mode.CONNECT:
      console.log("CONNECT mode");
      break;
  }

  redrawAll(state);

}

