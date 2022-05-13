'use strict';

async function onLoad() {
  while (true) {
    let sent_total = await fetch('/stats/sent_total').then(response => response.json()).then(data => data);
    document.getElementById("sent_total").innerHTML = sent_total;
    let clients_conn = await fetch('/stats/clients_conn').then(response => response.json()).then(data => data);
    document.getElementById("clients_conn").innerHTML = clients_conn;
    let received_total = await fetch('/stats/received_total').then(response => response.json()).then(data => data);
    document.getElementById("received_total").innerHTML = received_total;
    let subs_count = await fetch('/stats/subs_count').then(response => response.json()).then(data => data);
    document.getElementById("subs_count").innerHTML = subs_count;
    let clients_reg = await fetch('/stats/clients_reg').then(response => response.json()).then(data => data);
    document.getElementById("clients_reg").innerHTML = clients_reg;
    let broker = await fetch('/stats/broker').then(response => response.json()).then(data => data);
    document.getElementById("broker").innerHTML = broker["status"];
    setTimeout(function(){}, 5000);
  }
}

onLoad();
