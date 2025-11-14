# Telegram Bot Setup

Deze handleiding legt uit hoe je Telegram notificaties kunt instellen voor je NTRIP Caster.

## Stap 1: Maak een Telegram Bot aan

1. Open Telegram en zoek naar `@BotFather`
2. Start een chat en stuur `/newbot`
3. Volg de instructies:
   - Kies een naam voor je bot (bijv. "My NTRIP Caster")
   - Kies een username (moet eindigen op 'bot', bijv. "my_ntripcaster_bot")
4. BotFather geeft je een **Bot Token**, bijvoorbeeld:
   ```
   1234567890:ABCdefGHIjklMNOpqrsTUVwxyz
   ```
   Bewaar deze token!

## Stap 2: Verkrijg je Chat ID

1. Start een chat met je nieuwe bot (klik op de link die BotFather geeft)
2. Stuur een willekeurig bericht naar je bot (bijv. "Hallo")
3. Open in je browser:
   ```
   https://api.telegram.org/bot<JOUW_BOT_TOKEN>/getUpdates
   ```
   Vervang `<JOUW_BOT_TOKEN>` met de token uit stap 1

4. Je ziet JSON output zoals:
   ```json
   {
     "ok": true,
     "result": [{
       "update_id": 123456789,
       "message": {
         "message_id": 1,
         "from": {
           "id": 987654321,
           ...
         }
       }
     }]
   }
   ```

5. Zoek naar `"from": { "id": 987654321 }` - dit nummer is je **Chat ID**

## Stap 3: Configureer de NTRIP Caster

Bewerk `appsettings.json` in de Server directory:

```json
{
  "Telegram": {
    "Enabled": true,
    "BotToken": "1234567890:ABCdefGHIjklMNOpqrsTUVwxyz",
    "ChatId": "987654321"
  }
}
```

Of gebruik environment variables (aan te raden voor productie):

```bash
# In .env bestand
Telegram__Enabled=true
Telegram__BotToken=1234567890:ABCdefGHIjklMNOpqrsTUVwxyz
Telegram__ChatId=987654321
```

## Stap 4: Herstart de Server

Herstart de NTRIP Caster. Je zou nu een bericht moeten ontvangen:

```
✅ NTRIP Caster Started

System is now online and ready to accept connections.
Time: 2025-11-14 10:00:00 UTC
```

## Notificatie Types

De bot stuurt de volgende notificaties:

### Source Connected
```
📡 Source Connected

Mount Point: `test`
Time: 2025-11-14 10:05:00 UTC
```

### Source Disconnected
```
⚠️ Source Disconnected

Mount Point: `test`
Time: 2025-11-14 10:10:00 UTC
```

### System Started
```
✅ NTRIP Caster Started

System is now online and ready to accept connections.
Time: 2025-11-14 09:00:00 UTC
```

### Error Notifications
```
🔴 Error

Error message details here
Time: 2025-11-14 10:15:00 UTC
```

## Troubleshooting

### Bot stuurt geen berichten

1. Check of `Telegram:Enabled` op `true` staat
2. Controleer of de Bot Token correct is
3. Controleer of de Chat ID correct is (moet een nummer zijn, geen string)
4. Check de server logs voor Telegram errors
5. Zorg dat je een bericht naar de bot hebt gestuurd voordat je de Chat ID ophaalt

### "Unauthorized" error

De Bot Token is incorrect. Maak een nieuwe bot aan of check de token.

### Geen berichten ontvangen

1. Zorg dat je de bot hebt gestart door `/start` te sturen
2. Check of je het juiste Chat ID nummer hebt
3. Kijk in de server logs of er errors zijn

## Voor groepen/kanalen

Als je notificaties naar een Telegram groep of kanaal wilt sturen:

1. Voeg je bot toe aan de groep/kanaal
2. Maak de bot admin (voor kanalen verplicht)
3. Voor groepen: verkrijg de Chat ID door:
   - Een bericht in de groep te sturen
   - `getUpdates` te gebruiken en te zoeken naar `"chat": { "id": -123456789 }`
4. Groep Chat IDs zijn negatief (beginnen met -)

## Uitschakelen

Zet `Telegram:Enabled` op `false` in de configuratie om notificaties uit te schakelen.
