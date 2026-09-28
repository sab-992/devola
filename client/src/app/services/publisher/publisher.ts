import { Service } from '@angular/core';
import { Subscriber } from '@classes/subscriber/subscriber';
import { Subscription } from '@models/subscription';

@Service()
export class PublisherService {
    private listeners: Map<string, Set<Subscriber>> = new Map<string, Set<Subscriber>>();

    public subscribe(event: string, listener: Subscriber): Subscription {
        if (!this.listeners.has(event))
            this.listeners.set(event, new Set<Subscriber>());

        this.listeners.get(event)?.add(listener);
        return { event, subscriber: listener };
    }

    public unsubscribe(event: string, listener: Subscriber) {
        if (!this.listeners.has(event))
            return;

        this.listeners.get(event)?.delete(listener);

        if ((this.listeners.get(event)?.size as number) <= 0)
            this.listeners.delete(event);
    }

    public notify(event: string, data: object) {
        if (!this.listeners.has(event))
            return;

        this.listeners.get(event)?.forEach((subscriber: Subscriber) => { subscriber.update(data); });
    }
}
