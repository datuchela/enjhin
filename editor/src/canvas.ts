import { CANVAS_BACKGROUND, PARTICLE_COLOR, PARTICLE_COLOR_FOCUS } from "./constants.js";
import type { State, Color, Vector2 } from "./types.js";
import { getParticleById } from "./state.js";

export function redrawCanvas(state: State) {
  const { element: elements, data } = state;
  const { ctx } = elements;

  clearBackground(ctx, CANVAS_BACKGROUND);
  console.log(state);

  data.springs.forEach((spring) => {
    const particle1 = getParticleById(spring.particles[0], state);
    const particle2 = getParticleById(spring.particles[1], state);
    drawLine(ctx, particle1.position, particle2.position, "white");
  });

  data.particles.forEach((particle) => {
    let color = (particle.id === state.currentParticleId) ? PARTICLE_COLOR_FOCUS : PARTICLE_COLOR;
    drawCircle(ctx, particle.position, 5, color);
  });
}

export function drawCircle(ctx: CanvasRenderingContext2D, center: Vector2, radius: number, color: Color) {
  ctx.fillStyle = color;
  ctx.beginPath();
  ctx.arc(center.x, center.y, radius, 0, 2 * Math.PI);
  ctx.fill();
}

export function drawLine(ctx: CanvasRenderingContext2D, start: Vector2, end: Vector2, color: Color, lineWidth?: number) {
  ctx.strokeStyle = color;
  ctx.lineWidth = lineWidth ?? 1;
  ctx.moveTo(start.x, start.y);
  ctx.lineTo(end.x, end.y);
  ctx.stroke();
}

export function clearBackground(ctx: CanvasRenderingContext2D, color: Color) {
  ctx.fillStyle = color;
  ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
}
