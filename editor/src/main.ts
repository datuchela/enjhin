import { type State, Mode } from "./types.js";
import { CANVAS_HEIGHT, CANVAS_WIDTH } from "./constants.js";
import { redrawAll } from "./utils.js";
import { handleExportClick, handleModeClick, handleMouseClick, handleNewBodyClick } from "./handlers.js";

(() => {
  const canvas = document.getElementById("enjhin-editor") as HTMLCanvasElement | null;
  const listView = document.getElementById("list-view") as HTMLMenuElement | null;

  // Buttons
  const selectBtn = document.getElementById("mode-select") as HTMLButtonElement | null;
  const insertBtn = document.getElementById("mode-insert") as HTMLButtonElement | null;
  const moveBtn = document.getElementById("mode-move") as HTMLButtonElement | null;
  const connectBtn = document.getElementById("mode-connect") as HTMLButtonElement | null;
  const exportBtn = document.getElementById("export") as HTMLButtonElement | null;
  const newBodyBtn = document.getElementById("new-body") as HTMLButtonElement | null;

  if (!canvas) throw new Error("Couldn't get canvas");
  if (!listView) throw new Error("Couldn't get listView");
  if (!selectBtn) throw new Error("Couldn't get selectBtn");
  if (!insertBtn) throw new Error("Couldn't get insertBtn");
  if (!moveBtn) throw new Error("Couldn't get moveBtn");
  if (!connectBtn) throw new Error("Couldn't get connectBtn");
  if (!exportBtn) throw new Error("Couldn't get exportBtn");
  if (!newBodyBtn) throw new Error("Couldn't get newBodyBtn");

  const ctx = canvas.getContext("2d");
  if (!ctx) throw new Error(`Couldn't get 2d context from canvas: ${canvas}`);

  canvas.width = CANVAS_WIDTH;
  canvas.height = CANVAS_HEIGHT;

  const state: State = {
    mode: Mode.SELECT,
    data: {
      bodies: [],
      springs: [],
      particles: [],
    },
    currentBodyId: null,
    currentSpringId: null,
    currentParticleId: null,
    element: {
      canvas: canvas,
      ctx: ctx,
      listView: listView,
    },
    buttons: [
      { mode: Mode.SELECT, element: selectBtn },
      { mode: Mode.INSERT, element: insertBtn },
      { mode: Mode.MOVE, element: moveBtn },
      { mode: Mode.CONNECT, element: connectBtn },
    ],
    idDispatcher: {
      bodyIdCounter: 0,
      springIdCounter: 0,
      particleIdCounter: 0,
    }
  };

  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

  canvas.addEventListener("click", (e) => handleMouseClick(e, state));
  exportBtn.addEventListener("click", (e) => handleExportClick(e, state));
  newBodyBtn.addEventListener("click", () => handleNewBodyClick(state));
  state.buttons.forEach((btn) => {
    btn.element.addEventListener("click", () => handleModeClick(btn.mode, state));
  });

  redrawAll(state);
})();

