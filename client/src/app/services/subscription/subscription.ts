import { Injectable, inject } from '@angular/core';
import { Observable, ReplaySubject } from 'rxjs';
import { FeedSubscription } from '@models/feed-subscription';
import { HttpService } from '@services/http/http';
import { environment } from '@environments/environment';


@Injectable({ providedIn: 'root' })
export class SubscriptionService {
    private readonly m_http = inject(HttpService);

    public fetchSubscriptions(): Observable<FeedSubscription[]> {
        const sub = new ReplaySubject<FeedSubscription[]>(1);
        this.m_http.get<FeedSubscription[]>(this.buildPath("/subscriptions"), { next: (subscriptions) => { sub.next(subscriptions); sub.complete(); },
                                                                                error: (error) => sub.error(error) });
        return sub.asObservable();
    }

    public parseURL(url: string): { host: string, endpoint: string } {
        const END_OF_PREFIX_TOKEN = "://";
        const prefixIndex = url.indexOf(END_OF_PREFIX_TOKEN);
        const start = prefixIndex === -1 ? 0 : prefixIndex + END_OF_PREFIX_TOKEN.length;

        const searchIdx = url.slice(start).search(/[/?#]/);
        const pathStart = searchIdx === -1 ? -1 : start + searchIdx;

        const host = pathStart === -1 ? url.slice(start) : url.slice(start, pathStart);
        const endpoint = pathStart === -1 ? "/" : url.slice(pathStart);

        return { host, endpoint };
    }

    public saveSubscriptions(subscriptions: FeedSubscription[]): Observable<FeedSubscription[]> {
        const sub = new ReplaySubject<FeedSubscription[]>(1);
        this.m_http.put<FeedSubscription[], FeedSubscription[]>(this.buildPath("/subscriptions"), subscriptions, { next: (updatedSubscriptions) => { sub.next(updatedSubscriptions); sub.complete(); },
                                                                                error: (error) => sub.error(error) });
        return sub.asObservable();
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/rss${path}`;
    }
}
