import {
  type Particle,
  type ParticleId,
  type SoftBody,
  type SoftBodyId,
  type Spring,
  type SpringId,
  type State,
  Mode,
} from "./types.js";

export function getBodyById(id: SoftBodyId, state: State) {
  const { bodies } = state.data;

  const body = bodies.find((body) => body.id === id);
  if (!body) throw new Error(`Couldn't find body with id ${id}`);
  return body;
}

export function getParticleById(id: ParticleId, state: State) {
  const { particles } = state.data

  const particle = particles.find((particle) => particle.id === id);
  if (!particle) throw new Error(`Couldn't find particle with id ${id}`);
  return particle;
}

export function getSpringById(id: SpringId, state: State) {
  const { springs } = state.data

  const spring = springs.find((spring) => spring.id === id);
  if (!spring) throw new Error(`Couldn't find spring with id ${id}`);
  return spring;
}

export function addBody(body: SoftBody, state: State) {
  state.data.bodies.push(body);
}

export function addSpringToBody(spring: Spring, bodyId: SoftBodyId, state: State) {
  const body = getBodyById(bodyId, state);
  body.springs.push(spring.id);
}

export function addParticle(particle: Particle, bodyId: SoftBodyId, state: State) {
  const body = getBodyById(bodyId, state);
  state.data.particles.push(particle);
  body.particles.push(particle.id);
}

export function setCurrentBodyId(id: SoftBodyId, state: State) {
  state.currentBodyId = id;
}

export function setCurrentParticleId(id: ParticleId, state: State) {
  state.currentParticleId = id;
}

export function setCurrentSpringId(id: SpringId, state: State) {
  state.currentSpringId = id;
}

export function setMode(mode: Mode, state: State) {
  state.mode = mode;
}
