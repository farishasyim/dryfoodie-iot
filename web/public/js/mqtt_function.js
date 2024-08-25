const clientId = "mqttjs_" + Math.random().toString(16).substr(2, 8);

const host =
  "wss://d43ff0ab6db24ba296b1aed5d48bd702.s1.eu.hivemq.cloud:8884/mqtt";

const options = {
  username: "farishasyim",
  password: "FARfar101074",
  keepalive: 30,
  clientId,
  protocolId: "MQTT",
  protocolVersion: 4,
  clean: true,
  reconnectPeriod: 1000,
  connectTimeout: 30 * 1000,
  will: {
    topic: "WillMsg",
    payload: "Connection Closed abnormally..!",
    qos: 0,
    retain: false,
  },
  rejectUnauthorized: false,
};

console.log("connecting mqtt client");
const client = mqtt.connect(host, options);

client.on("error", (err) => {
  console.log(err);
  client.end();
});

client.on("connect", () => {
  console.log("client connected:" + clientId);
  client.subscribe("pour_value", { qos: 0 });
  client.subscribe("left", { qos: 0 });
});

function onPour(value) {
  client.publish("pour_value", value, {
    qos: 0,
    retain: false,
  });
}

client.on("message", (topic, message, packet) => {
  if (topic == "left") {
    var image = "public/empty.png";
    if (message >= 70 && message <= 100) {
      image = "public/high.png";
    } else if (message > 20 && message < 70) {
      image = "public/medium.png";
    } else if (message > 0 && message <= 20) {
      image = "public/low.png";
    }
    document.getElementById("percentage").innerText = `${message}%`;
    document.getElementById("image_percentage").src = image;
  } else if (topic == "pour_value") {
    // todo topic: pour value
  }
  console.log(
    "Received Message:= " + message.toString() + "\nOn topic:= " + topic
  );
});

client.on("close", () => {
  console.log(clientId + " disconnected");
});
