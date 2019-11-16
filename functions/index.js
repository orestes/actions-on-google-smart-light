const functions = require('firebase-functions');
const admin = require('firebase-admin');
const jwt = require('jsonwebtoken');

// TODO: Maybe only do this when you need access to the data
admin.initializeApp();

function getUserIdForAuthCode(code) {
  // TODO: Get corresponding user for the given auth code
  return 'fake-user-id';
}

function getToken(payload) {
  // TODO: Use secret from config/env
  return jwt.sign(payload, 'secret', {
    expiresIn: '1h', // TODO: Get expiration time from config/env
  });
}

const tokenHandler = (request, response) => {
  const authCode = request.body.code;
  const userId = getUserIdForAuthCode(authCode);

  console.log('Auth code matches user', { authCode, userId });

  // TODO: Add any metadata for the user
  const data = {
    authCode,
    userId,
  };

  const access_token = getToken(data);
  const refresh_token = getToken(data);

  console.log('Tokens generated', { access_token, refresh_token });

  response.send({
    access_token,
    refresh_token,
  });
};

function dec2hex(dec) {
  return (dec + Math.pow(16, 6)).toString(16).substr(-6);
}

function hex2dec(input) {
  return parseInt(input, 16);
}

function parseAsRGB(input) {
  const hex = dec2hex(input);
  const r = hex2dec(hex.substr(0, 2));
  const g = hex2dec(hex.substr(2, 2));
  const b = hex2dec(hex.substr(4, 2));

  return { r, g, b };
}

// TODO: Get this state from Firebase when the request handler loads
const devicesState = {
  'light-1': {
    on: true,
    online: true,
    brightness: 80,
    color: {
      name: 'cerulean',
      spectrumRGB: 31655,
    },
  },
};

const commands = [
  {
    ids: ['light-1'],
    status: 'SUCCESS',
    states: {
      on: true,
      online: true,
    },
  },
];

const intentMap = {
  'action.devices.SYNC': async (input, result) => {
    const saved = await admin
      .database()
      .ref('devices')
      .once(map);

    result.payload.devices = [
      {
        id: 'light-1',
        type: 'action.devices.types.LIGHT',
        traits: [
          'action.devices.traits.OnOff',
          'action.devices.traits.Brightness',
          'action.devices.traits.ColorSetting',
        ],
        name: {
          name: 'my smart light',
        },
        willReportState: false,
        attributes: {
          colorModel: 'rgb',
        },
      },
    ];
    return result;
  },
  'action.devices.EXECUTE': async (input, result) => {
    const command = input.payload.commands[0].execution[0];

    if (command.command === 'action.devices.commands.ColorAbsolute') {
      await admin
        .database()
        .ref('devices/light-1/state/color')
        .set({
          spectrum: command.params.color.spectrumRGB,
          rgb: parseAsRGB(command.params.color.spectrumRGB),
          hex: '#' + dec2hex(command.params.color.spectrumRGB),
          name: command.params.color.name,
        });

      result.payload.devices = devicesState; // TODO: Report online status and color
    }

    if (command.command === 'action.devices.commands.OnOff') {
      // TODO: Communicate with the device and get a result
      await admin
        .database()
        .ref('devices/light-1/state/on')
        .set(command.params.on);

      result.payload.devices = devicesState; // TODO: Report online status and color
    }

    if (command.command === 'action.devices.commands.BrightnessAbsolute') {
      // TODO: Communicate with the device and get a result
      await admin
        .database()
        .ref('devices/light-1/state/brightness')
        .set(command.params.brightness);

      result.payload.devices = devicesState; // TODO: Report online status and color
    }

    result.payload.commands = commands;

    return result;
  },
  'action.devices.QUERY': (input, result) => {
    // TODO: Read from Firebase
    result.payload.devices = devicesState;
    return result;
  },
};

const requestHandler = async (request, response) => {
  console.log('incoming request', JSON.stringify(request.body));

  const body = request.body;
  const requestId = body.requestId;
  const intent = body.inputs[0].intent;
  const agentUserId = 'fake-user-id'; // TODO: Get from JWT in Authorization HTTP header

  const wrapper = {
    requestId,
    payload: {
      agentUserId,
    },
  };

  const result = await intentMap[intent](body.inputs[0], wrapper);

  console.log('outgoing response', JSON.stringify(result));
  response.send(result);
};

exports.token = functions.https.onRequest(tokenHandler);
exports.request = functions.https.onRequest(requestHandler);
