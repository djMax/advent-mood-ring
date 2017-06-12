import { interpolateHclLong } from 'd3-interpolate';
import { color } from 'd3-color';

const interp = interpolateHclLong('#800080', '#FFA500');

function hex(n) {
  const v = n.toString(16);
  return `${'0'.substring(0, 2 - v.length)}${v}`;
}

const colors = [];
for (let i = 0; i < 256; i += 1) {
  const c = color(interp(i / 256));
  colors.push(`${hex(c.r)}${hex(c.g)}${hex(c.b)}`);
}

export function packedColors() {
  return colors.join('\n');
}
